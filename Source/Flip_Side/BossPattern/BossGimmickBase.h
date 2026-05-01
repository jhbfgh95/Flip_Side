#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BossDataTypes.h"
#include "GridTypes.h"
#include "BossGimmickBase.generated.h"

class ABossActor;
class ACoinActor;
class ABase_OtherActor;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossGimmickBase : public UObject
{
	GENERATED_BODY()

public:
	FBossGimmickData GimmickData;

	virtual void OnBattleStart(ABossActor* Boss) {}
	virtual void OnDamageCalculate(ABossActor* Boss, int32& InOutDamage) {}
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) {}
	virtual void OnTurnEnd(ABossActor* Boss) {}
	virtual void OnPlayerTurnStart(ABossActor* Boss) {}
	virtual void OnPlayerTurnEnd(ABossActor* Boss) {}
};