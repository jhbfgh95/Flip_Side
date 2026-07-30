#include "BossActor.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"

ABossActor::ABossActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BossRootComp"));
	RootComponent = BossRoot;

	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	BossMesh->SetupAttachment(RootComponent);

	FrontBackground = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontBackground"));
	FrontBackground->SetupAttachment(RootComponent);
	BottomBackground = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomBackground"));
	BottomBackground->SetupAttachment(RootComponent);
	LeftBackground = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftBackground"));
	LeftBackground->SetupAttachment(RootComponent);
	RightBackground = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightBackground"));
	RightBackground->SetupAttachment(RootComponent);

	FrontBackground->SetRelativeLocation(FVector(-60.f, -8890.f, 2440.f));
	FrontBackground->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	FrontBackground->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));

	BottomBackground->SetRelativeLocation(FVector(-60.f, 110.f, -4260.f));
	BottomBackground->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	BottomBackground->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));

	LeftBackground->SetRelativeLocation(FVector(-16070.f, 110.f, 2440.f));
	LeftBackground->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	LeftBackground->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));

	RightBackground->SetRelativeLocation(FVector(15930.f, 110.f, 2440.f));
	RightBackground->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	RightBackground->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));


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

	AnimInstance = BossMesh->GetAnimInstance();
	UpdateShieldEffect();
	BroadcastBossHUDDataChanged();

	if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ABossActor::BossMontageEnded);
    }
}

void ABossActor::InitializeFromBossData(const FBossBattleData& InData)
{
	ThemeID = InData.ThemeID;
	BossID = InData.BossID;
	BossName = InData.BossName;
	AttackPoint = InData.AttackPoint;
	MaxHP = InData.BossHP;
	CurrentHP = MaxHP;
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
	if(!DamageCauser) return 0;
	if(bIsDying) return 0;

	int32 FinalDamage = FMath::Max(0, Damage);

	if (ActiveGimmick)
		ActiveGimmick->OnDamageCalculate(this, FinalDamage);

	int32 ActualDamageToHP = FinalDamage;

	if(CurrentShield > 0)
	{
		const int32 ShieldDamage = FMath::Min(CurrentShield, FinalDamage);
		CurrentShield -= ShieldDamage;
		ActualDamageToHP = FinalDamage - ShieldDamage;

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

	if(CurrentHP <= 0 && !bIsDying)
	{
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
	if(CC.CCType == ECCTypes::None || CC.CCDuration <= 0) return;

	AppliedCC = CC;
	bIsOnCC = true;
	CCDuration = AppliedCC.CCDuration;

	UE_LOG(LogTemp, Warning, TEXT("[BossActor] CC Applied Type=%d Duration=%d"), static_cast<int32>(AppliedCC.CCType), CCDuration);
}

void ABossActor::SetMaxHP(int32 NewMaxHP)
{
	MaxHP = NewMaxHP;
	CurrentHP = NewMaxHP;
	BroadcastBossHUDDataChanged();
}


void ABossActor::RemoveCC()
{
	AppliedCC = FCCStructure();
	bIsOnCC = false;
	CCDuration = 0;

	UE_LOG(LogTemp, Warning, TEXT("[BossActor] CC Removed"));
}

bool ABossActor::ConsumeCCForBossTurn()
{
	if(!bIsOnCC)
	{
		return true;
	}

	if(AppliedCC.CCType == ECCTypes::Stun)
	{
		CCDuration--;

		UE_LOG(LogTemp, Warning, TEXT("[BossActor] Stunned. Skip boss attack. Remain=%d"), CCDuration);

		if(CCDuration <= 0)
		{
			RemoveCC();
		}

		return false;
	}

	CCDuration--;
	if(CCDuration <= 0)
	{
		RemoveCC();
	}

	return true;
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
	UE_LOG(LogTemp, Log, TEXT("[BossActor] Telegraph: BossID=%d Name=%s"),
		BossID, *BossName);
}

void ABossActor::PlayAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[BossActor] Attack: BossID=%d Name=%s"),
		BossID, *BossName);

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

   	AnimInstance->Montage_Play(SelectedPatternAnim);
}

void ABossActor::FinishBossAttack()
{
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
	UE_LOG(LogTemp, Warning, TEXT("[Boss] MontageEnded Target=%s Clear=%s Interrupted=%d"),
    *GetNameSafe(TargetMontage),
    *GetNameSafe(BossClearAnim),
    bInterrupted);
	if (TargetMontage == SelectedPatternAnim)
    {
        if(OnBossAttackEnded.IsBound()) OnBossAttackEnded.Broadcast();

    }
	else if(TargetMontage == BossClearAnim)
	{

		UE_LOG(LogTemp, Warning, TEXT("[Boss] BossClearAnim ended. Interrupted=%d"), bInterrupted);
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

void ABossActor::SetCurrentPatternInfo(int32 PatternIndex, const FBossPatternBattleData& PatternData)
{
	bHasCachedPatternInfo = true;
	CachedPatternIndex = PatternIndex;
	CachedPatternData = PatternData;

	BroadcastBossHUDDataChanged();
}

void ABossActor::SetCurrentHP(int32 NewHP)
{
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
		HUDData.PatternDescription = CachedPatternData.PatternDescription;
		HUDData.PatternDamage = AttackPoint;
		HUDData.PatternIcon = CachedPatternData.PatternIcon;
	}

	return HUDData;
}

void ABossActor::BroadcastBossHUDDataChanged()
{
	OnBossHUDDataChanged.Broadcast(GetBossHUDData());
}

void ABossActor::SetTextureOfBackgrounds(UTexture2D* Front,
		UTexture2D* Bottom,
		UTexture2D* Left,
		UTexture2D* Right)
{
	if(FrontBackground && BottomBackground && LeftBackground && RightBackground)
	{
		if(Front && Bottom && Left && Right)
		{
			UMaterialInstanceDynamic* MID1 = FrontBackground->CreateDynamicMaterialInstance(0);
			UMaterialInstanceDynamic* MID2 = BottomBackground->CreateDynamicMaterialInstance(0);
			UMaterialInstanceDynamic* MID3 = LeftBackground->CreateDynamicMaterialInstance(0);
			UMaterialInstanceDynamic* MID4 = RightBackground->CreateDynamicMaterialInstance(0);

			if(MID1) MID1->SetTextureParameterValue(TEXT("BackgroundTexture"), Front);
			if(MID2) MID2->SetTextureParameterValue(TEXT("BackgroundTexture"), Bottom);
			if(MID3) MID3->SetTextureParameterValue(TEXT("BackgroundTexture"), Left);
			if(MID4) MID4->SetTextureParameterValue(TEXT("BackgroundTexture"), Right);
		}
	}
}
