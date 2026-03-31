#include "BossPatternBase.h"
#include "BossManagerSubsystem.h"
#include "GridManagerSubsystem.h"

void UBossPatternBase::BuildTargetCells(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	TArray<FGridPoint>& OutCells, int32 PatternNum)
{
	if (!BossManager)
	{
		return;
	}

	UWorld* World = BossManager->GetWorld();
	if (!World)
	{
		return;
	}

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr)
	{
		return;
	}

	
	FAttackAreaSpec Spec;
	
	if(PatternData.IsValidIndex(PatternNum))
	{
		Spec = PatternData[PatternNum].PatternSpec;
	}

	GridMgr->BuildBossAttackCells(Spec, OutCells);
}

void UBossPatternBase::ExecutePattern(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	const TArray<FGridPoint>& InLockedCells,
	const TArray<ACoinActor*>& InLockedTargets, int32 PatternNum)
{
}