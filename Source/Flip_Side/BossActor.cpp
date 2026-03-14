#include "BossActor.h"
#include "Component_Status.h"

ABossActor::ABossActor()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

int32 ABossActor::GetPatternCount() const
{
	return Patterns.Num();
}

UBossPatternBase* ABossActor::GetPattern(int32 Index) const
{
	if (!Patterns.IsValidIndex(Index))
	{
		return nullptr;
	}

	return Patterns[Index];
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
}