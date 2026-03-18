#include "BossActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component_Status.h"

ABossActor::ABossActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(BossMesh);

	StatusComp = CreateDefaultSubobject<UComponent_Status>(TEXT("StatusComp"));
}

void ABossActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABossActor::InitializeFromBossData(const FBossData& InData)
{
	ThemeID = InData.ThemeID;
	BossID = InData.BossID;
	BossName = InData.BossName;
	AttackPoint = InData.AttackPoint;

	if (StatusComp)
	{
		StatusComp->SetHP(InData.BossHP);
	}
	if (UAnimInstance* AnimInstance = BossMesh->GetAnimInstance())
    {
        AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ABossActor::AttackMontageEnded);
    }
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

bool ABossActor::IsDead() const
{
	if (!StatusComp)
	{
		return false;
	}

	return StatusComp->GetHP() <= 0;
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

	UAnimInstance* AnimInstance = BossMesh->GetAnimInstance();
    if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AnimInstacne"));
	}

   	AnimInstance->Montage_Play(SelectedPatternAnim);
}

void ABossActor::AttackMontageEnded(UAnimMontage * TargetMontage, bool bInterrupted)
{
	if(OnBossAttackEnded.IsBound())
	{
		OnBossAttackEnded.Broadcast();
	}
}

void ABossActor::SetPatternAnim(UAnimMontage * TargetMontage)
{
	if(TargetMontage)
	{
		SelectedPatternAnim = TargetMontage;
	}
}