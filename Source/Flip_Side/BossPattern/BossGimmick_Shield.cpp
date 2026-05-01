#include "BossGimmick_Shield.h"
#include "BossActor.h"

void UBossGimmick_Shield::OnBattleStart(ABossActor* Boss)
{
	if (!Boss) return;

	Boss->InitShield(GimmickData.ParamIntA);
}