#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Poison.generated.h"

UCLASS()
class FLIP_SIDE_API UBossGimmick_Poison : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) override;
	virtual void OnPlayerPhaseStart(ABossActor* Boss) override;
	virtual void OnPlayerPhaseEnd(ABossActor* Boss) override;

private:
	FTimerHandle PoisonTimerHandle;
	// 값 = 남은 지속 턴수. 플레이어 턴이 끝날 때마다 1씩 깎이고 0이 되면 제거됩니다.
	TMap<TWeakObjectPtr<ACoinActor>, int32> PoisonedCoins;
};
