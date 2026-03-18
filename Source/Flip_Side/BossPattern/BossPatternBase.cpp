#include "BossPatternBase.h"

void UBossPatternBase::BuildTargetCells(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	TArray<FGridPoint>& OutCells, int32 PatternNum)
{
}

void UBossPatternBase::ExecutePattern(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	const TArray<FGridPoint>& InLockedCells,
	const TArray<ACoinActor*>& InLockedTargets, int32 PatternNum)
{
}