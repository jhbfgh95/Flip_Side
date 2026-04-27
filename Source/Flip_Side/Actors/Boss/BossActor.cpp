#include "BossActor.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "W_BossHP.h"

ABossActor::ABossActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BossRootComp"));
	RootComponent = BossRoot;

	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	BossMesh->SetupAttachment(RootComponent);

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
	if(BossHPWidgetClass)
	{
		BossHpWidget = CreateWidget<UW_BossHP>(GetWorld(), BossHPWidgetClass);
		if(BossHpWidget)
		{
			BossHpWidget->AddToViewport();
			BossHpWidget->SetVisibility(ESlateVisibility::Visible);
			BossHpWidget->SetBossName(BossName);
			BossHpWidget->InitBossHp(MaxHP);
			if(MaxShield > 0)
			{
				if(CurrentShield <= 0)
				{
					CurrentShield = MaxShield;
				}
				BossHpWidget->InitBossShield(MaxShield);
			}
			ApplyCachedPatternInfoToWidget();
		}
	}
	UpdateShieldEffect();

	if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ABossActor::BossMontageEnded);
    }
}

void ABossActor::InitializeFromBossData(const FBossData& InData)
{
	ThemeID = InData.ThemeID;
	BossID = InData.BossID;
	BossName = InData.BossName;
	AttackPoint = InData.AttackPoint;
	MaxHP = InData.BossHP;
	CurrentHP = MaxHP;

	if(BossHpWidget)
	{
		BossHpWidget->SetBossName(BossName);
		BossHpWidget->InitBossHp(MaxHP);
		if(MaxShield > 0)
		{
			if(CurrentShield <= 0)
			{
				CurrentShield = MaxShield;
			}
			BossHpWidget->InitBossShield(MaxShield);
		}
		ApplyCachedPatternInfoToWidget();
	}
	UpdateShieldEffect();
}

void ABossActor::ApplyDamage(int32 Damage, AActor* DamageCauser)
{
	ApplyDamageAndReturnHPDamage(Damage, DamageCauser);
}

int32 ABossActor::ApplyDamageAndReturnHPDamage(int32 Damage, AActor* DamageCauser)
{
	if(!DamageCauser) return 0;

	int32 FinalDamage = FMath::Max(0, Damage);
	int32 ActualDamageToHP = FinalDamage;

	if(CurrentShield > 0)
	{
		const int32 ShieldDamage = FMath::Min(CurrentShield, FinalDamage);
		CurrentShield -= ShieldDamage;
		ActualDamageToHP = FinalDamage - ShieldDamage;

		if(BossHpWidget && ShieldDamage > 0)
		{
			BossHpWidget->ChangeCurrentShield(-ShieldDamage);
		}

		if(ActualDamageToHP <= 0)
		{
			if(BossHitAnim && AnimInstance) AnimInstance->Montage_Play(BossHitAnim);
			UpdateShieldEffect();
			return 0;
		}
	}

	if(ActualDamageToHP <= 0) return 0;

	CurrentHP -= ActualDamageToHP;

	if(BossHpWidget)
	{
		BossHpWidget->ChangeCurrentHp(-ActualDamageToHP);
	}

	if(CurrentHP <= 0)
	{
		if(BossHpWidget)
		{
			BossHpWidget->ShowClearImage();
		}

		if(AnimInstance && BossClearAnim)
		{
			AnimInstance->Montage_Play(BossClearAnim);
		}
	}
	else
	{
		if(BossHitAnim && AnimInstance) AnimInstance->Montage_Play(BossHitAnim);
	}
	UpdateShieldEffect();

	return ActualDamageToHP;
}

int32 ABossActor::ApplyShieldOnlyDamage(int32 Damage, AActor* DamageCauser)
{
	if(!DamageCauser) return 0;

	const int32 ShieldDamage = FMath::Min(CurrentShield, FMath::Max(0, Damage));
	if(ShieldDamage <= 0) return 0;

	CurrentShield -= ShieldDamage;

	if(BossHpWidget)
	{
		BossHpWidget->ChangeCurrentShield(-ShieldDamage);
	}

	if(BossHitAnim && AnimInstance) AnimInstance->Montage_Play(BossHitAnim);
	UpdateShieldEffect();

	return ShieldDamage;
}

void ABossActor::ApplyShieldHeal(int32 Heal, AActor* HealCauser)
{
	if(!HealCauser || MaxShield <= 0) return;

	const int32 OldShield = CurrentShield;
	CurrentShield = FMath::Clamp(CurrentShield + FMath::Max(0, Heal), 0, MaxShield);

	const int32 ActualHealedAmount = CurrentShield - OldShield;
	if(ActualHealedAmount > 0 && BossHpWidget)
	{
		BossHpWidget->ChangeCurrentShield(ActualHealedAmount);
	}
	UpdateShieldEffect();
}

void ABossActor::ApplyCC(const FCCStructure& CC)
{
	if(CC.CCType == ECCTypes::None || CC.CCDuration <= 0) return;

	AppliedCC = CC;
	bIsOnCC = true;
	CCDuration = AppliedCC.CCDuration;

	UE_LOG(LogTemp, Warning, TEXT("[BossActor] CC Applied Type=%d Duration=%d"), static_cast<int32>(AppliedCC.CCType), CCDuration);
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

bool ABossActor::GetPatternDataList(TArray<FPatternData>& OutPatternDataList) const
{
	OutPatternDataList.Reset();

	if(!Pattern)
	{
		return false;
	}

	OutPatternDataList = Pattern->PatternData;
	return OutPatternDataList.Num() > 0;
}

bool ABossActor::GetPatternData(int32 PatternIndex, FPatternData& OutPatternData) const
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
		UE_LOG(LogTemp, Warning, TEXT("No AnimInstacne"));
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

void ABossActor::BossMontageEnded(UAnimMontage * TargetMontage, bool bInterrupted)
{
	if (TargetMontage == SelectedPatternAnim)
    {
        if(OnBossAttackEnded.IsBound()) OnBossAttackEnded.Broadcast();

    }
	else if(TargetMontage == BossClearAnim)
	{
		if(OnBossDead.IsBound()) OnBossDead.Broadcast();
	}
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

void ABossActor::SetCurrentPatternInfo(int32 PatternIndex, const FPatternData& PatternData)
{
	bHasCachedPatternInfo = true;
	CachedPatternIndex = PatternIndex;
	CachedPatternData = PatternData;

	ApplyCachedPatternInfoToWidget();
}

void ABossActor::ApplyCachedPatternInfoToWidget()
{
	if(!BossHpWidget)
	{
		return;
	}

	if(!bHasCachedPatternInfo)
	{
		return;
	}

	const int32 PatternDisplayIndex = CachedPatternIndex + 1;
	const int32 FinalDamage = AttackPoint + CachedPatternData.Damage;

	BossHpWidget->SetPatternInfo(
		PatternDisplayIndex,
		CachedPatternData.PatternName,
		CachedPatternData.PatternDescription,
		FinalDamage,
		CachedPatternData.PatternIcon
	);
}
