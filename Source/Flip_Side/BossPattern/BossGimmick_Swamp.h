#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "GridTypes.h"
#include "BossGimmick_Swamp.generated.h"

class AGridActor;

USTRUCT()
struct FSwampCell
{
	GENERATED_BODY()

	FGridPoint GridPoint;
	int32 RemainingTurns = 0;
};

UCLASS()
class FLIP_SIDE_API UBossGimmick_Swamp : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnBeforePatternExecute(ABossActor* Boss, FBossTurnContext& Context) override;
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) override;
	virtual void OnTurnEnd(ABossActor* Boss) override;

	bool IsSwampCell(const FGridPoint& Cell) const;
	const TArray<FSwampCell>& GetActiveSwamps() const { return ActiveSwamps; }

private:
	TArray<FSwampCell> ActiveSwamps;

	void InstallSwamp(ABossActor* Boss, const TArray<FGridPoint>& Cells);
};
