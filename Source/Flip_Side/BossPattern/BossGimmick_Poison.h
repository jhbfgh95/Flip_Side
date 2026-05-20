#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Poison.generated.h"

class UW_PoisonTimerUI;

UCLASS()
class FLIP_SIDE_API UBossGimmick_Poison : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) override;
	virtual void OnPlayerTurnStart(ABossActor* Boss) override;
	virtual void OnPlayerTurnEnd(ABossActor* Boss) override;

private:
	FTimerHandle PoisonTimerHandle;
	TArray<TWeakObjectPtr<ACoinActor>> PoisonedCoins;

	UPROPERTY()
	UW_PoisonTimerUI* PoisonTimerWidget = nullptr;
};
