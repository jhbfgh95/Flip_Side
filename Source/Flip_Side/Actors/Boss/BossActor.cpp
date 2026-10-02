#include "BossActor.h"
#include "BossManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Actors/Component_Status.h"
#include "Actors/CoinActor.h"
#include "BossGimmick_Swamp.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

ABossActor::ABossActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BossRootComp"));
	RootComponent = BossRoot;
	DebuffComponent = CreateDefaultSubobject<UDebuffComponent>(TEXT("DebuffComponent"));
	DebuffComponent->bAttackOnly = true;
	DebuffComponent->ConsumeIncomingCC.BindUObject(this, &ABossActor::TryConsumeIncomingCC);
	CCEffectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("CCEffectLocation"));
	CCEffectLocation->SetupAttachment(RootComponent);
	CCDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CCDisplayMesh"));
	CCDisplayMesh->SetupAttachment(CCEffectLocation);
	CCDisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CCDisplayMesh->SetGenerateOverlapEvents(false);
	CCDisplayMesh->SetCastShadow(false);
	CCDisplayMesh->SetVisibility(false);

	BossFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BossFloorMesh"));
	BossFloorMesh->SetupAttachment(BossRoot);
	BossFloorMesh->SetMobility(EComponentMobility::Movable);
	BossFloorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BossFloorMesh->SetGenerateOverlapEvents(false);

	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	BossMesh->SetupAttachment(RootComponent);
	// 코인 발판(3x3) 위에 올라가도록 축소 + 띄우기 - Simulate에서 확인한 값
	BossMesh->SetRelativeLocation(FVector(0.f, 0.f, 900.f));
	BossMesh->SetRelativeScale3D(FVector(20.f, 20.f, 20.f));

	ShieldEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShieldEffect"));
	ShieldEffectComponent->SetupAttachment(BossMesh);
	ShieldEffectComponent->SetAutoActivate(false);
	ShieldEffectComponent->SetVisibility(true);

	BossSelfEffectLoc = CreateDefaultSubobject<USceneComponent>(TEXT("SelfEffectLocation"));
	BossSelfEffectLoc->SetupAttachment(RootComponent);
}

void ABossActor::BeginPlay()
{
	Super::BeginPlay();
	SetTargetOutlineEnabled(false);
	if (IsValid(DebuffComponent))
	{
		DebuffComponent->OnCCChanged.AddUniqueDynamic(this, &ABossActor::HandleCCVisualChanged);
		DebuffComponent->OnDebuffChanged.AddUObject(this, &ABossActor::HandleDebuffChanged);
		HandleCCVisualChanged(DebuffComponent->GetCCType());
	}

	AnimInstance = IsValid(BossMesh) ? BossMesh->GetAnimInstance() : nullptr;
	UpdateShieldEffect();
	BroadcastBossHUDDataChanged();

	if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ABossActor::BossMontageEnded);
    }
}

void ABossActor::SetTargetOutlineEnabled(bool bEnabled)
{
	if (!IsValid(BossMesh)) return;
	BossMesh->SetCustomDepthStencilValue(211);
	BossMesh->SetRenderCustomDepth(bEnabled && CurrentHP > 0 && !bBossDeathFinished && !IsHidden());
}

void ABossActor::InitializeFromBossData(const FBossBattleData& InData)
{
	ThemeID = InData.ThemeID;
	BossID = InData.BossID;
	BossName = InData.BossName;
	AttackPoint = InData.AttackPoint;
	MaxHP = InData.BossHP;
	CurrentHP = MaxHP;
	PillarDamageBonus = 0;
	bPillarTurnStarted = false;
	bHPDamagedThisTurn = false;
	StageMultiplierStat = InData.StageMultiplierStat;
	StageMultiplierGimmick = InData.StageMultiplierGimmick;

	if (!InData.ClearAnim.IsNull())
		BossClearAnim = InData.ClearAnim.LoadSynchronous();
	if (!InData.HitAnim.IsNull())
		BossHitAnim = InData.HitAnim.LoadSynchronous();

	UpdateShieldEffect();
	BroadcastBossHUDDataChanged();
}

void ABossActor::ApplyDamage(int32 Damage, AActor* DamageCauser)
{
	ApplyDamageAndReturnHPDamage(Damage, DamageCauser);
}

int32 ABossActor::ApplyDamageAndReturnHPDamage(int32 Damage, AActor* DamageCauser)
{
	if(!IsValid(DamageCauser)) return 0;
	if(bIsDying || CurrentHP <= 0) return 0;

	int32 FinalDamage = FMath::Max(0, Damage);

	if (ActiveGimmick)
		ActiveGimmick->OnDamageCalculate(this, FinalDamage);

	FinalDamage = FMath::Max(0, FinalDamage);
	int32 ActualDamageToHP = FinalDamage;

	if(CurrentShield > 0)
	{
		const ACoinActor* Attacker = Cast<ACoinActor>(DamageCauser);
		const int32 ShieldMultiplier = IsValid(Attacker) && IsValid(Attacker->StatComponent)
			? Attacker->StatComponent->GetShieldDamageMultiplier() : 1;
		const int64 ShieldDamageBudget = static_cast<int64>(FinalDamage) * ShieldMultiplier;
		const int32 ShieldDamage = static_cast<int32>(FMath::Min<int64>(CurrentShield, ShieldDamageBudget));
		CurrentShield -= ShieldDamage;
		// 보호막을 깨고 남은 피해는 원래 배율로 HP에 적용합니다. 소수 피해는 버립니다.
		ActualDamageToHP = static_cast<int32>((ShieldDamageBudget - ShieldDamage) / ShieldMultiplier);

		if(ActualDamageToHP <= 0)
		{
			PlayHitAnimation();
			UpdateShieldEffect();
			BroadcastBossHUDDataChanged();
			return 0;
		}
	}

	if(ActualDamageToHP <= 0) return 0;

	CurrentHP -= ActualDamageToHP;
	bHPDamagedThisTurn = true;
	PillarDamageBonus = 0;

	if(CurrentHP <= 0 && !bIsDying)
	{
		if (IsValid(DebuffComponent)) DebuffComponent->DisableForDeath();
		SetTargetOutlineEnabled(false);
		if(AnimInstance && BossClearAnim)
		{
			bIsDying = true;
			if(OnBossDeathStarted.IsBound()) OnBossDeathStarted.Broadcast();
			AnimInstance->Montage_Play(BossClearAnim);
		}
	}
	else if(CurrentHP > 0)
	{
		PlayHitAnimation();
	}
	UpdateShieldEffect();
	BroadcastBossHUDDataChanged();

	return ActualDamageToHP;
}

int32 ABossActor::ApplyShieldOnlyDamage(int32 Damage, AActor* DamageCauser)
{
	if(!DamageCauser) return 0;

	const int32 ShieldDamage = FMath::Min(CurrentShield, FMath::Max(0, Damage));
	if(ShieldDamage <= 0) return 0;

	CurrentShield -= ShieldDamage;

	PlayHitAnimation();
	UpdateShieldEffect();
	BroadcastBossHUDDataChanged();

	return ShieldDamage;
}

void ABossActor::AddGimmick(UBossGimmickBase* InGimmick)
{
	if (InGimmick)
	{
		GimmickList.Add(InGimmick);
	}
}


void ABossActor::InitShield(int32 ShieldValue)
{
	MaxShield = ShieldValue;
	CurrentShield = ShieldValue;
	UpdateShieldEffect();
	BroadcastBossHUDDataChanged();
}

void ABossActor::ApplyShieldHeal(int32 Heal, AActor* HealCauser)
{
	if(!HealCauser || MaxShield <= 0) return;

	const int32 OldShield = CurrentShield;
	CurrentShield = FMath::Clamp(CurrentShield + FMath::Max(0, Heal), 0, MaxShield);

	const int32 ActualHealedAmount = CurrentShield - OldShield;
	UpdateShieldEffect();
	if (ActualHealedAmount > 0)
	{
		BroadcastBossHUDDataChanged();
	}
}

void ABossActor::ApplyCC(const FCCStructure& CC)
{
	if (!bIsDying && IsValid(DebuffComponent)) DebuffComponent->ApplyCC(CC.CCType, CC.CCDuration);
}

bool ABossActor::TryConsumeIncomingCC(const FStatusEffectInstance& Effect) { return false; }

int32 ABossActor::GetAttackPoint() const
{
	return static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(AttackPoint) +
		(IsValid(DebuffComponent) ? DebuffComponent->GetAttackModifier() : 0), 0, MAX_int32));
}

void ABossActor::BeginPillarTurn()
{
	const int32 PreviousBonus = PillarDamageBonus;
	// 첫 턴은 보너스 없이 시작하며, 이후 완료된 턴에 HP 감소가 없었을 때만 누적합니다.
	if (bPillarTurnStarted && !bHPDamagedThisTurn && CurrentHP > 0)
	{
		PillarDamageBonus = static_cast<int32>(FMath::Min<int64>(static_cast<int64>(PillarDamageBonus) + 1, MAX_int32));
	}
	bPillarTurnStarted = true;
	bHPDamagedThisTurn = false;
	if (PreviousBonus != PillarDamageBonus) BroadcastBossHUDDataChanged();
}

int32 ABossActor::GetDamageWithPillarBonus(int32 Damage) const
{
	return static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(FMath::Max(0, Damage)) +
		PillarDamageBonus, 0, MAX_int32));
}

void ABossActor::HandleDebuffChanged(const FStatusEffectInstance& Effect, bool bGameplayChanged)
{
	// 레거시 BP 읽기 전용 필드는 저장소가 아니라 공통 컴포넌트 상태의 표시용 미러입니다.
	AppliedCC = FCCStructure();
	if (IsValid(DebuffComponent))
		for (const FStatusEffectInstance& E : DebuffComponent->GetDebuffs())
			if (E.CCType != ECCTypes::None) { AppliedCC.CCType = E.CCType; AppliedCC.CCDuration = E.RemainingTurns; }
	bIsOnCC = AppliedCC.CCType != ECCTypes::None;
	CCDuration = AppliedCC.CCDuration;
}

void ABossActor::HandleCCVisualChanged(ECCTypes CCType)
{
	if (IsValid(CCDisplayMesh))
	{
		CCDisplayMesh->SetVisibility(false);
		UGameInstance* GI = GetGameInstance();
		UDataManagerSubsystem* Data = IsValid(GI) ? GI->GetSubsystem<UDataManagerSubsystem>() : nullptr;
		FDebuffDefinitionData Definition;
		// 지원하는 CC 모두 DB 아이콘을 표시하며 기절 몽타주는 별도로 유지합니다.
		const int32 CCBuffTypeID = CCType == ECCTypes::Blind ? DebuffTypeID::Blind :
			CCType == ECCTypes::Stun ? DebuffTypeID::Stun : INDEX_NONE;
		if (CCBuffTypeID != INDEX_NONE && IsValid(Data) && Data->TryGetDebuff(CCBuffTypeID, Definition) &&
			IsValid(Definition.Icon) && IsValid(CCDisplayMesh->GetStaticMesh()))
		{
			if (!IsValid(CCDisplayMaterial)) CCDisplayMaterial = CCDisplayMesh->CreateDynamicMaterialInstance(0);
			if (IsValid(CCDisplayMaterial))
			{
				CCDisplayMaterial->SetTextureParameterValue(TEXT("CC_Icon"), Definition.Icon);
				CCDisplayMaterial->SetVectorParameterValue(TEXT("CC_Color"), Definition.Color);
				CCDisplayMesh->SetVisibility(true);
			}
		}
	}
	UAnimInstance* BossAnim = IsValid(BossMesh) ? BossMesh->GetAnimInstance() : nullptr;
	if (IsValid(BossAnim) && IsValid(StunMontage))
	{
		if (CCType == ECCTypes::Stun) BossAnim->Montage_Play(StunMontage);
		else if (BossAnim->Montage_IsPlaying(StunMontage)) BossAnim->Montage_Stop(0.1f, StunMontage);
	}
	OnCCVisualChanged(CCType); // 보스 BP: CC 아이콘 / 추가 연출 / None 해제.
}

void ABossActor::SetMaxHP(int32 NewMaxHP)
{
	MaxHP = NewMaxHP;
	CurrentHP = NewMaxHP;
	BroadcastBossHUDDataChanged();
}


void ABossActor::RemoveCC()
{
	if (IsValid(DebuffComponent)) DebuffComponent->ClearCC();
}

bool ABossActor::ConsumeCCForBossPhase()
{
	// 수명은 SettingPhase에서만 차감합니다. BossManager의 기존 완료 통지는 유지합니다.
	return !IsStunned();
}

int32 ABossActor::GetPatternCount() const
{
	if(!Pattern)
	{
		return 0;
	}

	return Pattern->PatternData.Num();
}

UBossPatternBase* ABossActor::GetPattern() const
{
	if (!Pattern)
	{
		return nullptr;
	}

	return Pattern;
}

bool ABossActor::GetPatternDataList(TArray<FBossPatternBattleData>& OutPatternDataList) const
{
	OutPatternDataList.Reset();

	if(!Pattern)
	{
		return false;
	}

	OutPatternDataList = Pattern->PatternData;
	return OutPatternDataList.Num() > 0;
}

bool ABossActor::GetPatternData(int32 PatternIndex, FBossPatternBattleData& OutPatternData) const
{
	if(!Pattern || !Pattern->PatternData.IsValidIndex(PatternIndex))
	{
		return false;
	}

	OutPatternData = Pattern->PatternData[PatternIndex];
	return true;
}

FVector ABossActor::GetSelfEffectLocation() const
{
	
	if (BossSelfEffectLoc)
	{
		return BossSelfEffectLoc->GetComponentLocation();
	}
		
	return GetActorLocation();
	
}

void ABossActor::PlayTelegraph()
{
}

void ABossActor::PlayAttack()
{

    if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AnimInstance"));
		FinishBossAttack();
		return;
	}

	if (!SelectedPatternAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("No SelectedPatternAnim"));
		FinishBossAttack();
		return;
	}

    if (AnimInstance->Montage_Play(SelectedPatternAnim) <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossNotify] Attack montage failed: %s"), *GetNameSafe(SelectedPatternAnim.Get()));
        FinishBossAttack();
    }
}

void ABossActor::FinishBossAttack()
{
	if (UWorld* World = GetWorld())
	{
		if (UBossManagerSubsystem* Manager = World->GetSubsystem<UBossManagerSubsystem>(); IsValid(Manager) && Manager->GetCurrentBoss() == this)
		{
			Manager->FinishCurrentAttack(true);
		}
	}
	if(OnBossAttackEnded.IsBound())
	{
		OnBossAttackEnded.Broadcast();
	}
}

void ABossActor::PlayHitAnimation()
{
	if(BossHitAnim && AnimInstance)
	{
		AnimInstance->Montage_Play(BossHitAnim);
	}
}

void ABossActor::BossMontageEnded(UAnimMontage * TargetMontage, bool bInterrupted)
{
	if (TargetMontage == SelectedPatternAnim)
    {
        UWorld* World = GetWorld();
        UBossManagerSubsystem* Manager = IsValid(World) ? World->GetSubsystem<UBossManagerSubsystem>() : nullptr;
        if (!IsValid(Manager) || Manager->GetCurrentBoss() != this || !Manager->IsAttackExecuting()) return;
        Manager->FinishCurrentAttack(bInterrupted);
        FinishBossAttack();
    }
	else if(TargetMontage == BossClearAnim)
	{

		if(bInterrupted) return;

		FinishBossClearAnimation();
	}
}

void ABossActor::FinishBossClearAnimation()
{
	if(bBossDeathFinished) return;

	bBossDeathFinished = true;

	if(BossMesh)
	{
		BossMesh->bPauseAnims = true;
	}

	BossDeadEffect();

	if(BossDeadEffectDelay <= 0.0f)
	{
		BroadcastBossDeadAfterEffect();
		return;
	}

	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BossDeadEffectTimerHandle,
			this,
			&ABossActor::BroadcastBossDeadAfterEffect,
			BossDeadEffectDelay,
			false
		);
		return;
	}

	BroadcastBossDeadAfterEffect();
}

void ABossActor::BroadcastBossDeadAfterEffect()
{
	if(OnBossDead.IsBound()) OnBossDead.Broadcast();
}

void ABossActor::UpdateShieldEffect()
{
	if(!ShieldEffectComponent) return;

	if(CurrentShield > 0)
	{
		ShieldEffectComponent->SetVisibility(true);
		ShieldEffectComponent->Activate();
	}
	else
	{
		ShieldEffectComponent->Deactivate();
		ShieldEffectComponent->SetVisibility(false);
	}
}

void ABossActor::SetPatternAnim(UAnimMontage * TargetMontage)
{
	if(TargetMontage)
	{
		SelectedPatternAnim = TargetMontage;
	}
}

bool ABossActor::ApplyCurrentPatternBuff(UComponent_Status* TargetStatus, FStatusEffectInstance Buff)
{
	if (!IsValid(TargetStatus) || !bHasCachedPatternInfo || CachedPatternIndex == INDEX_NONE ||
		Buff.BuffTypeID == INDEX_NONE || Buff.Polarity != EStatusPolarity::Buff)
	{
		return false;
	}
	// 디버프 경로는 변경하지 않으며, 버프만 현재 패턴 정보를 값으로 보관합니다.
	Buff.SourceType = EStatusEffectSourceType::Boss;
	Buff.SourceDataID = GetBossID();
	Buff.SourcePatternIndex = CachedPatternIndex;
	Buff.SourcePatternIcon = CachedPatternData.PatternIcon;
	return TargetStatus->AddStatusEffect(Buff);
}

void ABossActor::SetCurrentPatternInfo(int32 PatternIndex, const FBossPatternBattleData& PatternData)
{
	bHasCachedPatternInfo = true;
	CachedPatternIndex = PatternIndex;
	CachedPatternData = PatternData;

	BroadcastBossHUDDataChanged();
}

void ABossActor::SetCurrentHP(int32 NewHP)
{
	if (NewHP < CurrentHP)
	{
		bHPDamagedThisTurn = true;
		PillarDamageBonus = 0;
	}
	CurrentHP = NewHP;
	BroadcastBossHUDDataChanged();
}

void ABossActor::SetCurrentShield(int32 NewShield)
{
	CurrentShield = NewShield;
	BroadcastBossHUDDataChanged();
}

FBossHUDData ABossActor::GetBossHUDData() const
{
	FBossHUDData HUDData;
	HUDData.BossName = BossName;
	HUDData.CurrentHP = CurrentHP;
	HUDData.MaxHP = MaxHP;
	HUDData.CurrentShield = CurrentShield;
	HUDData.MaxShield = MaxShield;
	HUDData.bHasPatternInfo = bHasCachedPatternInfo;

	if (bHasCachedPatternInfo)
	{
		HUDData.PatternDisplayIndex = CachedPatternIndex + 1;
		HUDData.PatternName = CachedPatternData.PatternName;
		HUDData.bIsGimmick = CachedPatternData.bIsGimmick;
		HUDData.PatternDescription = CachedPatternData.PatternDescription;
		HUDData.PatternDamage = AttackPoint;
		HUDData.bPatternNoDamage = CachedPatternData.bNoDamage;
		HUDData.bShowPatternDamage = CachedPatternData.bNoDamage
			|| CachedPatternData.GimmickType != EBossGimmickType::RoleTarget;
		// 표시용 조회에서는 실행 훅을 호출하지 않습니다(턴/기믹 상태 변경 방지).
		if (!CachedPatternData.bNoDamage && HUDData.bShowPatternDamage)
		{
			for (const UBossGimmickBase* Gimmick : GimmickList)
			{
				if (!IsValid(Gimmick)) continue;
				const UBossGimmick_Swamp* Swamp = Cast<UBossGimmick_Swamp>(Gimmick);
				if (IsValid(Swamp) && CachedPatternIndex >= 0 && CachedPatternIndex <= 2)
				{
					HUDData.PatternDamage = Swamp->GetPatternDamage(CachedPatternIndex);
					HUDData.bHasConditionalPatternDamage = CachedPatternIndex == 2;
					if (HUDData.bHasConditionalPatternDamage)
					{
						HUDData.ConditionalPatternDamage = Swamp->GetPatternDamage(CachedPatternIndex, true);
					}
					break;
				}
			}
		}
		if (!CachedPatternData.bNoDamage)
		{
			HUDData.PatternDamage = GetDamageWithPillarBonus(HUDData.PatternDamage);
			if (HUDData.bHasConditionalPatternDamage)
				HUDData.ConditionalPatternDamage = GetDamageWithPillarBonus(HUDData.ConditionalPatternDamage);
		}
		HUDData.PatternIcon = CachedPatternData.PatternIcon;
	}

	return HUDData;
}

void ABossActor::BroadcastBossHUDDataChanged()
{
	OnBossHUDDataChanged.Broadcast(GetBossHUDData());
}
