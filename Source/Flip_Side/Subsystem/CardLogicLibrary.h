#pragma once

#include "CoreMinimal.h"
#include "DataTypes/CardTypes.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"

class ACoinActor;
class UDataManagerSubsystem;

// 판정과 보정값 계산만 수행합니다. 버프 등록/제거와 턴당 지급은 StageCard가 관리합니다.
class FLIP_SIDE_API FCardLogicLibrary
{
public:
    static bool Evaluate(
        const FCardData& Card,
        const TArray<FCoinOnGridInfo>& FieldCoins,
        const TMap<int32, int32>& SourceSlotByCoinID,
        int32 BattleEntryGold,
        const FGridPoint& PromotionGrid,
        UDataManagerSubsystem* DataManager,
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& OutModifiers);
};
