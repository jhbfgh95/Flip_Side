#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Poison.generated.h"

/**
 * 낙인(구 독) 기믹.
 * - 패턴에 맞은 코인에게 낙인을 "끝까지" 유지되는 개별 디버프로 추가합니다(재타격 시 새 낙인이 0부터 따로 쌓임).
 * - 보스 페이즈에 진입할 때(OnPlayerPhaseEnd) 코인의 모든 낙인의 지난 턴 수 합 x 기본 피해를 줍니다.
 */
UCLASS()
class FLIP_SIDE_API UBossGimmick_Poison : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) override;
	virtual void OnPlayerPhaseEnd(ABossActor* Boss) override;
};
