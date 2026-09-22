#include "BossGimmick_Blind.h"
#include "Actors/DebuffComponent.h"
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
			// 아이콘/UI는 보스 출처를 보존하고, 지속시간은 공통 디버프 컴포넌트가 관리합니다.
			if (UDebuffComponent* Debuffs = StatusComp->GetDebuffComponent(); IsValid(Debuffs))
				Debuffs->ApplyCC(BlindCC.CCType, BlindCC.CCDuration, EStatusEffectSourceType::Boss, Boss->GetBossID());
		}
	}
}
