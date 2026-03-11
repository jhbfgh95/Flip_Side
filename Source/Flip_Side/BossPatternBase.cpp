#include "BossPatternBase.h"

void UBossPatternBase::BuildTargetCells(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	TArray<FGridPoint>& OutCells)
{
}

void UBossPatternBase::ExecutePattern(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	const TArray<FGridPoint>& InLockedCells,
	const TArray<ACoinActor*>& InLockedTargets)
{
}