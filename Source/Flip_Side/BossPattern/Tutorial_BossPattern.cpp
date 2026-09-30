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
	// 피해/기믹만 적용합니다. VFX는 BossVFX 노티파이에서 실행합니다.
	UBossPatternBase::ExecutePattern(Boss, Context, InLockedTargets, InLockedOthers);
}
