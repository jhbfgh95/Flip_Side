#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Blind.generated.h"

UCLASS()
class FLIP_SIDE_API UBossGimmick_Blind : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) override;
};
