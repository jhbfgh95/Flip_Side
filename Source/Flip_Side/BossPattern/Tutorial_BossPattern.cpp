#include "Tutorial_BossPattern.h"
#include "BossManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "BossActor.h"
#include "Tutorial_BossActor.h"

void UTutorial_BossPattern::ExecutePattern(
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

	if(PatternNum == 0 || PatternNum == 1)
	{
		TArray<ACoinActor*> ActualTargets;
		BossManager->GetCoinsOnCells(InLockedCells, ActualTargets);

		const int32 FinalDamage = Boss ? (Boss->GetAttackPoint() + PatternData[PatternNum].Damage) : PatternData[PatternNum].Damage;
		BossManager->ApplyDamageToLockedTargets(ActualTargets, FinalDamage);
	}
    else
    {
        ATutorial_BossActor* TutoBoss = Cast<ATutorial_BossActor>(Boss);
        
        if (TutoBoss)
        {
            TutoBoss->ApplyShieldHeal(TutoBoss->GetAttackPoint(), TutoBoss);
        }
    }

}