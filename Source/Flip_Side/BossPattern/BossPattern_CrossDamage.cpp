#include "BossPattern_CrossDamage.h"
#include "BossManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "BossActor.h"

void UBossPattern_CrossDamage::BuildTargetCells(
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
	/*
	Spec.Pattern = EAttackAreaPattern::CrossOnCell;
	Spec.AnchorMode = EAreaAnchor::UseAnchorCell;
	Spec.AnchorCell = AnchorCell;
	Spec.ParamA = HalfX;
	Spec.ParamB = HalfY;
	*/
	
	if(PatternData.IsValidIndex(PatternNum))
	{
		Spec = PatternData[PatternNum].PatternSpec;
	}

	GridMgr->BuildBossAttackCells(Spec, OutCells);
}

void UBossPattern_CrossDamage::ExecutePattern(
	UBossManagerSubsystem* BossManager,
	ABossActor* Boss,
	const TArray<FGridPoint>& InLockedCells,
	const TArray<ACoinActor*>& InLockedTargets, int32 PatternNum)
{
	if (!BossManager)
	{
		return;
	}

	if(!PatternData.IsValidIndex(PatternNum)) return;

	const int32 FinalDamage = Boss ? (Boss->GetAttackPoint() + PatternData[PatternNum].Damage) : PatternData[PatternNum].Damage;
	BossManager->ApplyDamageToLockedTargets(InLockedTargets, FinalDamage);
}