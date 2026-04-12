#include "BossActor.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "W_BossHP.h"

ABossActor::ABossActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BossRootComp"));
	RootComponent = BossRoot;

	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	BossMesh->SetupAttachment(RootComponent);

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
			BossHpWidget->InitBossHp(MaxHP);
		}
	}

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
}

void ABossActor::ApplyDamage(int32 Damage, AActor* DamageCauser)
{
	if(!DamageCauser) return;

	if(CurrentHP - Damage <= 0)
	{
		//죽는 애니메이션 + 위젯
		if(AnimInstance && BossClearAnim)
		{
			BossHpWidget->ChangeCurrentHp(-Damage);
			AnimInstance->Montage_Play(BossClearAnim);
			return;
		}
	}

	if(BossHitAnim) AnimInstance->Montage_Play(BossHitAnim);

	BossHpWidget->ChangeCurrentHp(-Damage);
	CurrentHP -= Damage;
}

int32 ABossActor::GetPatternCount() const
{
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

void ABossActor::SetPatternAnim(UAnimMontage * TargetMontage)
{
	if(TargetMontage)
	{
		SelectedPatternAnim = TargetMontage;
	}
}
