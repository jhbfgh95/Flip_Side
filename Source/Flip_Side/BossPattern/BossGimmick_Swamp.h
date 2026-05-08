#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "BossGimmick_Swamp.generated.h"

class AGridActor;

USTRUCT()
struct FSwampCell
{
	GENERATED_BODY()

	FGridPoint GridPoint;
	int32 RemainingTurns = 0;
	EWeaponClass SwampType = EWeaponClass::None;
};

UCLASS()
class FLIP_SIDE_API UBossGimmick_Swamp : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnPlayerTurnStart(ABossActor* Boss) override;
	virtual void OnCoinLanded(ABossActor* Boss, FBossTurnContext& Context) override;
	virtual void OnBeforePatternExecute(ABossActor* Boss, FBossTurnContext& Context) override;
	virtual void OnTurnEnd(ABossActor* Boss) override;

	bool IsSwampCell(const FGridPoint& Cell) const;
	const TArray<FSwampCell>& GetActiveSwamps() const { return ActiveSwamps; }

	static FLinearColor GetSwampColor(EWeaponClass Type);

private:
	TArray<FSwampCell> ActiveSwamps;

	void InstallSwamp(ABossActor* Boss, const TArray<FGridPoint>& Cells);
};
