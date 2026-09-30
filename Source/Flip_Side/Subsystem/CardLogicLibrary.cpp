#include "Subsystem/CardLogicLibrary.h"

#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
#include "DataTypes/CoinStatDataTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystem/DataManagerSubsystem.h"

bool FCardLogicLibrary::Evaluate(
    const FCardData& Card,
    const TArray<FCoinOnGridInfo>& FieldCoins,
    const TMap<int32, int32>& SourceSlotByCoinID,
    int32 BattleEntryGold,
    const FGridPoint& PromotionGrid,
    UDataManagerSubsystem* DataManager,
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& OutModifiers)
{
    OutModifiers.Reset();
    TArray<ACoinActor*> Coins;
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        if (IsValid(Info.CoinActor) && IsValid(Info.CoinActor->StatComponent) && !Info.CoinActor->StatComponent->IsDead())
            Coins.AddUnique(Info.CoinActor);
    }
    if (Coins.IsEmpty()) return false;

    FCoinCardModifiers Base;
    Base.AttackAdd = Card.AttackAdd;
    Base.BehaviorAdd = Card.BehaviorAdd;
    Base.CountAdd = Card.CountAdd;
    Base.RangeAdd = Card.RangeAdd;
    Base.AbilityRangeAdd = Card.AbilityRangeAdd;
    Base.ExtraActions = Card.ExtraActions;
    Base.bLifeSteal = Card.bLifeSteal;

    switch (Card.CardID)
    {
    case 1:
        for (ACoinActor* Coin : Coins)
            if (Coin->GetCoinDecidedFace() != EFaceState::Front) return false;
        for (ACoinActor* Coin : Coins) OutModifiers.Add(Coin, Base);
        break;
    case 2:
    {
        if (!IsValid(DataManager)) return false;
        TMap<ACoinActor*, int32> BaseRanges;
        int32 HighRangeCount = 0;
        for (ACoinActor* Coin : Coins)
        {
            FFaceData Weapon;
            if (!DataManager->TryGetWeapon(Coin->GetCoinFaceID(), Weapon)) continue;
            const int32 Range = GetWeaponAreaRange(Weapon.AttackAreaSpec);
            BaseRanges.Add(Coin, Range);
            if (Range >= Card.TriggerRange) ++HighRangeCount;
        }
        if (HighRangeCount < Card.TriggerCount) return false;
        for (const auto& Pair : BaseRanges)
        {
            FCoinCardModifiers Modifier = Base;
            Modifier.BehaviorAdd = Card.BehaviorAdd == -1 ? Pair.Value : Card.BehaviorAdd;
            OutModifiers.Add(Pair.Key, Modifier);
        }
        break;
    }
    case 3:
        if (PromotionGrid.GridX < 0 || PromotionGrid.GridY < 0) return false;
        for (ACoinActor* Coin : Coins)
            if (Coin->GetCoinFaceID() == Card.RequiredWeaponID && Coin->GetDecidedGrid() == PromotionGrid)
                OutModifiers.Add(Coin, Base);
        break;
    case 4:
    {
        const FCardGoldTierData* Best = nullptr;
        for (const FCardGoldTierData& Tier : Card.GoldTiers)
            if (BattleEntryGold >= Tier.MinimumGold && (!Best || Tier.MinimumGold > Best->MinimumGold)) Best = &Tier;
        if (!Best) return false;
        for (ACoinActor* Coin : Coins) OutModifiers.Add(Coin, Best->Modifiers);
        break;
    }
    case 5:
        if (Coins.Num() != Card.TriggerCount) return false;
        for (ACoinActor* Coin : Coins) OutModifiers.Add(Coin, Base);
        break;
    case 6:
    {
        TMap<int32, int32> Counts;
        for (ACoinActor* Coin : Coins)
            if (const int32* Slot = SourceSlotByCoinID.Find(Coin->GetCoinID()); Slot && *Slot != INDEX_NONE)
                ++Counts.FindOrAdd(*Slot);
        for (ACoinActor* Coin : Coins)
            if (const int32* Slot = SourceSlotByCoinID.Find(Coin->GetCoinID());
                Slot && *Slot != INDEX_NONE && Counts.FindRef(*Slot) >= Card.TriggerCount)
                OutModifiers.Add(Coin, Base);
        break;
    }
    default:
        return false;
    }
    return !OutModifiers.IsEmpty();
}
