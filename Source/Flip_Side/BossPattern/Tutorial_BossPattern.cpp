#include "Tutorial_BossPattern.h"
#include "GridManagerSubsystem.h"
#include "BossActor.h"
#include "GridActor.h"
#include "Tutorial_BossActor.h"
#include "Actors/Others/Base_OtherActor.h"

void UTutorial_BossPattern::ExecutePattern(
	ABossActor* Boss,
	const TArray<FGridPoint>& InLockedCells,
	const TArray<ACoinActor*>& InLockedTargets,
	const TArray<ABase_OtherActor*>& InLockedOthers,
	int32 PatternNum)
{
	if (!Boss)
	{
		return;
	}

	UWorld* World = Boss->GetWorld();
	if (!World)
	{
		return;
	}

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr)
	{
		return;
	}

	if(!PatternData.IsValidIndex(PatternNum)) return;

	if(PatternNum == 0 || PatternNum == 1)
	{
		FVector2D AnchorGrid2D = GridMgr->GetGridActor(PatternData[PatternNum].PatternSpec.AnchorCell)->GetGridWorldXY();
		FVector SpawnLocation = FVector(AnchorGrid2D.X, AnchorGrid2D.Y, -80.f);

		const int32 FinalDamage = Boss->GetAttackPoint() + PatternData[PatternNum].Damage;
		
		PlayPatternEffect(PatternNum, SpawnLocation);

		ExecuteDamage(InLockedTargets, InLockedOthers, Boss, FinalDamage);
	}
    else
    {
        ATutorial_BossActor* TutoBoss = Cast<ATutorial_BossActor>(Boss);
        
        if (TutoBoss)
        {
            TutoBoss->ApplyShieldHeal(TutoBoss->GetAttackPoint(), TutoBoss);
			PlayPatternEffect(PatternNum, TutoBoss->GetSelfEffectLocation());
        }
    }
}