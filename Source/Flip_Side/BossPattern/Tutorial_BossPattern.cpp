#include "Tutorial_BossPattern.h"
#include "GridManagerSubsystem.h"
#include "BossActor.h"
#include "GridActor.h"
#include "Tutorial_BossActor.h"
#include "Actors/Others/Base_OtherActor.h"

void UTutorial_BossPattern::ExecutePattern(
	ABossActor* Boss,
	FBossPhaseContext& Context,
	const TArray<ACoinActor*>& InLockedTargets,
	const TArray<ABase_OtherActor*>& InLockedOthers)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr) return;

	const int32 PatternNum = Context.CurrentPatternIndex;
	if (!PatternData.IsValidIndex(PatternNum)) return;

	UBossPatternBase::ExecutePattern(Boss, Context, InLockedTargets, InLockedOthers);

	if (PatternData.IsValidIndex(PatternNum))
	{
		FVector SpawnLocation;
		if (PatternData[PatternNum].bNoDamage)
		{
			ATutorial_BossActor* TutoBoss = Cast<ATutorial_BossActor>(Boss);
			SpawnLocation = TutoBoss ? TutoBoss->GetSelfEffectLocation() : Boss->GetActorLocation();
		}
		else
		{
			FVector2D AnchorGrid2D = GridMgr->GetGridActor(PatternData[PatternNum].PatternSpec.AnchorCell)->GetGridWorldXY();
			SpawnLocation = FVector(AnchorGrid2D.X, AnchorGrid2D.Y, -80.f);
		}
		PlayPatternEffect(PatternNum, SpawnLocation);
	}
}