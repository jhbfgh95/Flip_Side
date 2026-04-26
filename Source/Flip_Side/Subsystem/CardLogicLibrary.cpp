#include "Subsystem/CardLogicLibrary.h"

#include "CoinActor.h"
#include "Component_Status.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/AttackAreaTypes.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "FlipSide_Enum.h"

// ===== BuildLogicTable =====

void FCardLogicLibrary::BuildLogicTable(TMap<int32, FCardLogicFn>& OutTable)
{
    OutTable.Add(1, [](const FCardData& Card, const TArray<FCoinOnGridInfo>& Coins, TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods, UDataManagerSubsystem* DM)
    {
        Card_Encore(Card, Coins, Mods, DM);
    });

    OutTable.Add(2, [](const FCardData& Card, const TArray<FCoinOnGridInfo>& Coins, TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods, UDataManagerSubsystem* DM)
    {
        Card_LongRangeAmplifier(Card, Coins, Mods, DM);
    });

    // 3: 프로모션 - StageCardWSubsystem에서 직접 처리 (HighlightedGrid 필요)
    OutTable.Add(3, [](const FCardData&, const TArray<FCoinOnGridInfo>&, TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>&, UDataManagerSubsystem*) {});

    // 4: 황금 기회 - 미구현
    OutTable.Add(4, [](const FCardData&, const TArray<FCoinOnGridInfo>&, TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>&, UDataManagerSubsystem*) {});

    OutTable.Add(5, [](const FCardData& Card, const TArray<FCoinOnGridInfo>& Coins, TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods, UDataManagerSubsystem* DM)
    {
        Card_OneForAll(Card, Coins, Mods, DM);
    });

    OutTable.Add(6, [](const FCardData& Card, const TArray<FCoinOnGridInfo>& Coins, TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods, UDataManagerSubsystem* DM)
    {
        Card_Alliance(Card, Coins, Mods, DM);
    });
}

// ===== Utilities =====

void FCardLogicLibrary::AddMods(
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
    ACoinActor* Coin,
    int32 AttackAdd, int32 BehaviorAdd, int32 RangeAdd,
    bool bLifeSteal, int32 ExtraActions)
{
    if (!IsValid(Coin)) return;

    FCoinCardModifiers& M = Mods.FindOrAdd(Coin);
    M.AttackAdd    += AttackAdd;
    M.BehaviorAdd  += BehaviorAdd;
    M.RangeAdd     += RangeAdd;
    M.ExtraActions += ExtraActions;
    if (bLifeSteal) M.bLifeSteal = true;
}

bool FCardLogicLibrary::TryGetFaceData(ACoinActor* Coin, UDataManagerSubsystem* DM, FFaceData& OutFace)
{
    if (!IsValid(Coin) || !DM) return false;
    return DM->TryGetWeapon(Coin->GetCoinFaceID(), OutFace);
}

int32 FCardLogicLibrary::GetRangeValue(const FFaceData& Face)
{
    // AttackRange(range_x/y) is unused (all 0 in DB). Read actual range from AttackAreaSpec.
    // SingleCell: param_b is range, CircleOnCell: param_a is radius, else: max(param_a, param_b)
    const FAttackAreaSpec& Spec = Face.AttackAreaSpec;
    switch (Spec.Pattern)
    {
    case EAttackAreaPattern::SingleCell:
        return Spec.ParamB;
    case EAttackAreaPattern::CircleOnCell:
        return Spec.ParamA;
    default:
        return FMath::Max(Spec.ParamA, Spec.ParamB);
    }
}

// ===== Card Logic =====

void FCardLogicLibrary::Card_Encore(
    const FCardData& Card,
    const TArray<FCoinOnGridInfo>& FieldCoins,
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
    UDataManagerSubsystem* DM)
{
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        if (!IsValid(Info.CoinActor)) continue;
        if (Info.CoinActor->GetCoinDecidedFace() != EFaceState::Front) return;
    }

    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        if (!IsValid(Info.CoinActor)) continue;
        AddMods(Mods, Info.CoinActor, 0, 0, 0, false, Card.ExtraActions);
        break;
    }
}

void FCardLogicLibrary::Card_LongRangeAmplifier(
    const FCardData& Card,
    const TArray<FCoinOnGridInfo>& FieldCoins,
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
    UDataManagerSubsystem* DM)
{
    int32 CountHighRange = 0;
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        FFaceData Face;
        if (!TryGetFaceData(Info.CoinActor, DM, Face)) continue;
        if (GetRangeValue(Face) >= 3) ++CountHighRange;
    }

    if (CountHighRange < Card.TriggerCount) return;

    // behavior_add == -1: sentinel meaning "equal to each coin's range value"
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        FFaceData Face;
        if (!TryGetFaceData(Info.CoinActor, DM, Face)) continue;

        const int32 R = GetRangeValue(Face);
        const int32 ActualBehaviorAdd = (Card.BehaviorAdd == -1) ? R : Card.BehaviorAdd;
        AddMods(Mods, Info.CoinActor, Card.AttackAdd, ActualBehaviorAdd, Card.RangeAdd);
    }
}

void FCardLogicLibrary::ApplyPromotion(
    const FCardData& Card,
    const TArray<FCoinOnGridInfo>& FieldCoins,
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
    UDataManagerSubsystem* DM,
    const FGridPoint& HighlightedGrid)
{
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        if (!IsValid(Info.CoinActor)) continue;
        if (Info.CoinActor->GetCoinFaceID() != 3) continue;
        if (!(Info.CoinActor->GetDecidedGrid() == HighlightedGrid)) continue;

        AddMods(Mods, Info.CoinActor,
            Card.AttackAdd, Card.BehaviorAdd, Card.RangeAdd, Card.bLifeSteal);
    }
}

void FCardLogicLibrary::Card_OneForAll(
    const FCardData& Card,
    const TArray<FCoinOnGridInfo>& FieldCoins,
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
    UDataManagerSubsystem* DM)
{
    ACoinActor* OnlyCoin = nullptr;
    int32 ValidCount = 0;

    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        if (!IsValid(Info.CoinActor)) continue;
        ++ValidCount;
        OnlyCoin = Info.CoinActor;
        if (ValidCount > Card.TriggerCount) break;
    }

    if (ValidCount == Card.TriggerCount && IsValid(OnlyCoin))
    {
        AddMods(Mods, OnlyCoin, Card.AttackAdd, Card.BehaviorAdd, Card.RangeAdd);
    }
}

void FCardLogicLibrary::Card_Alliance(
    const FCardData& Card,
    const TArray<FCoinOnGridInfo>& FieldCoins,
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Mods,
    UDataManagerSubsystem* DM)
{
    TMap<EWeaponClass, TArray<ACoinActor*>> ClassCoins;

    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        FFaceData Face;
        if (!TryGetFaceData(Info.CoinActor, DM, Face)) continue;
        ClassCoins.FindOrAdd(Face.WeaponType).Add(Info.CoinActor);
    }

    for (auto& Pair : ClassCoins)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Alliance] Class=%d, Count=%d, TriggerCount=%d"),
            (int32)Pair.Key, Pair.Value.Num(), Card.TriggerCount);

        if (Pair.Value.Num() < Card.TriggerCount) continue;

        for (ACoinActor* Coin : Pair.Value)
        {
            if (Pair.Key == EWeaponClass::Deal)
            {
                AddMods(Mods, Coin, 0, Card.BehaviorAdd, 0);
            }
            else if (Pair.Key == EWeaponClass::Tank)
            {
                // TODO: MaxHP +1 (미구현)
            }
            else
            {
                AddMods(Mods, Coin, Card.AttackAdd, 0, 0);
            }
        }
    }
}