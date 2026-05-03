#include "BossGimmick_Blind.h"
#include "BossActor.h"
#include "CoinActor.h"
#include "Component_Status.h"
#include "CoinDataTypes.h"
#include "FlipSide_Enum.h"
#include "Actors/Others/Base_OtherActor.h"

void UBossGimmick_Blind::OnPatternExecute(
	ABossActor* Boss,
	const TArray<FGridPoint>& LockedCells,
	const TArray<ACoinActor*>& LockedTargets,
	const TArray<ABase_OtherActor*>& LockedOthers)
{
	if (!Boss) return;

	FCCStructure BlindCC;
	BlindCC.CCType = ECCTypes::Blind;
	BlindCC.CCDuration = GimmickData.ParamIntA > 0 ? GimmickData.ParamIntA : 2;

	for (ACoinActor* Coin : LockedTargets)
	{
		if (!IsValid(Coin)) continue;

		UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
		if (StatusComp)
		{
			StatusComp->ApplyCC(BlindCC);
		}
	}
}
