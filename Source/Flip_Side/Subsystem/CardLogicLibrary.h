#pragma once

#include "CoreMinimal.h"
#include "DataTypes/CardTypes.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"

class ACoinActor;
class UDataManagerSubsystem;
struct FFaceData;

// 카드 로직 함수 시그니처
using FCardLogicFn = TFunction<void(
    const FCardData&,                                          // 카드 수치 파라미터
    const TArray<FCoinOnGridInfo>&,                           // 필드 코인 목록
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>&,    // modifier 맵 출력
    UDataManagerSubsystem*                                    // 무기 데이터 조회용
)>;

class FLIP_SIDE_API FCardLogicLibrary
{
public:
    // 카드 ID -> 로직 함수 테이블 빌드 (StageCardWSubsystem Initialize 시 1회 호출)
    static void BuildLogicTable(TMap<int32, FCardLogicFn>& OutTable);

    // 프로모션 카드: 빛나는 칸에 있는 쇠파이프 코인에만 버프
    static void ApplyPromotion(
        const FCardData& Card,
        const TArray<FCoinOnGridInfo>& FieldCoins,
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
        UDataManagerSubsystem* DM,
        const FGridPoint& HighlightedGrid);

private:
    // ===== 카드별 로직 =====
    static void Card_Encore(
        const FCardData& Card,
        const TArray<FCoinOnGridInfo>& FieldCoins,
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
        UDataManagerSubsystem* DM);

    static void Card_LongRangeAmplifier(
        const FCardData& Card,
        const TArray<FCoinOnGridInfo>& FieldCoins,
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
        UDataManagerSubsystem* DM);

    static void Card_OneForAll(
        const FCardData& Card,
        const TArray<FCoinOnGridInfo>& FieldCoins,
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
        UDataManagerSubsystem* DM);

    static void Card_Alliance(
        const FCardData& Card,
        const TArray<FCoinOnGridInfo>& FieldCoins,
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
        UDataManagerSubsystem* DM);

    // 유틸: modifier 맵에 값 누적
    static void AddMods(
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
        ACoinActor* Coin,
        int32 AttackAdd, int32 BehaviorAdd, int32 RangeAdd,
        bool bLifeSteal = false, int32 ExtraActions = 0);

    // 유틸: 코인의 현재 면 무기 데이터 조회
    static bool TryGetFaceData(ACoinActor* Coin, UDataManagerSubsystem* DM, FFaceData& OutFace);

    // 유틸: 무기 사거리 (max(x,y))
    static int32 GetRangeValue(const FFaceData& Face);
};