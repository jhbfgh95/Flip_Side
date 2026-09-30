// StageCardWSubsystem.cpp
#include "Subsystem/StageCardWSubsystem.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"

#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Subsystem/CardLogicLibrary.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Component_Status.h"
#include "CoinActor.h"
#include "GridActor.h"

#include "DataTypes/GridTypes.h"
#include "DataTypes/WeaponDataTypes.h"

bool UStageCardWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
        return false;

    UWorld* World = Cast<UWorld>(Outer);
    if (!World)
        return false;

    const FString MapName = World->GetName();
    return MapName.Contains(TEXT("L_Stage"));
}

void UStageCardWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    HandCards.SetNum(HandCount);
    bHasCard.SetNum(HandCount);

    for (int32 i = 0; i < HandCount; ++i)
    {
        bHasCard[i] = false;
        HandCards[i] = FCardData();
    }

    GridSubsys = Collection.InitializeDependency<UGridManagerSubsystem>();
    ActingManager = Collection.InitializeDependency<UBattleLevelActingWSubsystem>();
    Collection.InitializeDependency<UCoinManagementWSubsystem>();
}

void UStageCardWSubsystem::Deinitialize()
{
    SettingDoSettingPhase();
    GridSubsys = nullptr;
    ActingManager = nullptr;
    CrossingGI = nullptr;
    DM = nullptr;

    Super::Deinitialize();
}

void UStageCardWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (!InWorld.IsGameWorld())
        return;

    GridSubsys = InWorld.GetSubsystem<UGridManagerSubsystem>();
    ActingManager = InWorld.GetSubsystem<UBattleLevelActingWSubsystem>();

    if (UGameInstance* GI = InWorld.GetGameInstance())
    {
        CrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>();
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
        const UMoneyGISubsystem* Money = GI->GetSubsystem<UMoneyGISubsystem>();
        const int32 CurrentGold = IsValid(Money) ? Money->GetCurrentMoney() : 0;
        BattleEntryGold = IsValid(CrossingGI) ? CrossingGI->ConsumeBattleEntryGold(CurrentGold) : CurrentGold;
    }

    // ===== ���� ���� ����ȭ =====
    RefreshHandFromGI();

    // ===== ���� ���� Ÿ�̹� ī�� ó��=====
    ClearAllModifiers();
}

void UStageCardWSubsystem::RefreshHandFromGI()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return;

    UCrossingLevelGISubsystem* LocalCrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>();
    if (!LocalCrossingGI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] CrossingLevelGISubsystem is null"));
        return;
    }
    CrossingGI = LocalCrossingGI;
    ClearAllModifiers();
    CardPrice = 0;

    const TArray<int32> IDs = CrossingGI->GetBattleCardIDs();

    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        const int32 CardID = IDs.IsValidIndex(Slot) ? IDs[Slot] : -1;

        if (CardID < 0)
        {
            ClearSlot(Slot, /*bNotify=*/true);
            continue;
        }

        FCardData CardData;
        if (!TryLoadCardData(CardID, CardData))
        {
            UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] TryLoadCardData failed. CardID=%d Slot=%d"), CardID, Slot);
            ClearSlot(Slot, /*bNotify=*/true);
            continue;
        }

        HandCards[Slot] = CardData;
        bHasCard[Slot] = true;
        CardPrice += CardData.Price;
        OnHandCardSet.Broadcast(Slot, CardData);
    }

    if (GetCardCount() == 0)
    {
        TestCardGenerate();
        return;
    }

    OnBattleCardDataChanged.Broadcast();
}

bool UStageCardWSubsystem::TestCardGenerate()
{
    // 실제 장착 카드가 하나라도 있으면 테스트 데이터로 덮어쓰지 않습니다.
    if (GetCardCount() > 0)
    {
        return false;
    }

    UWorld* World = GetWorld();
    UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
    UDataManagerSubsystem* DataManager = IsValid(GameInstance)
        ? GameInstance->GetSubsystem<UDataManagerSubsystem>()
        : nullptr;
    if (!IsValid(DataManager) || !DataManager->IsCacheReady())
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCard] TestCardGenerate failed: DataManager cache is unavailable."));
        OnBattleCardDataChanged.Broadcast();
        return false;
    }

    TArray<FCardData> AllCards;
    if (!DataManager->TryGetAllCards(AllCards))
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCard] TestCardGenerate failed: card DB data could not be loaded."));
        OnBattleCardDataChanged.Broadcast();
        return false;
    }

    AllCards.RemoveAll([](const FCardData& CardData)
    {
        return CardData.CardID < 0;
    });
    AllCards.Sort([](const FCardData& Left, const FCardData& Right)
    {
        return Left.CardID < Right.CardID;
    });

    const int32 GeneratedCardCount = FMath::Min(HandCount, AllCards.Num());
    CardPrice = 0;
    for (int32 SlotIndex = 0; SlotIndex < GeneratedCardCount; ++SlotIndex)
    {
        HandCards[SlotIndex] = AllCards[SlotIndex];
        bHasCard[SlotIndex] = true;
        CardPrice += HandCards[SlotIndex].Price;
        OnHandCardSet.Broadcast(SlotIndex, HandCards[SlotIndex]);
    }

    if (GeneratedCardCount < HandCount)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("[StageCard] TestCardGenerate created %d/%d card types."),
            GeneratedCardCount,
            HandCount
        );
    }

    OnBattleCardDataChanged.Broadcast();
    return GeneratedCardCount > 0;
}

bool UStageCardWSubsystem::TryLoadCardData(int32 CardID, FCardData& Out) const
{
    UWorld* World = GetWorld();
    if (!World) return false;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return false;

    UDataManagerSubsystem* LocalDM = GI->GetSubsystem<UDataManagerSubsystem>();
    if (!LocalDM) return false;

    return LocalDM->TryGetCard(CardID, Out);
}

bool UStageCardWSubsystem::ChangeHandCard(int32 CardID, int32 CardSlot)
{
    if (!HandCards.IsValidIndex(CardSlot) || !bHasCard.IsValidIndex(CardSlot))
    {
        UE_LOG(LogTemp, Warning, TEXT("[TargetCardChange] CardSlot must be 0-2. Slot=%d"), CardSlot);
        return false;
    }
    FCardData Card;
    if (!TryLoadCardData(CardID, Card))
    {
        UE_LOG(LogTemp, Warning, TEXT("[TargetCardChange] Unknown CardID=%d"), CardID);
        return false;
    }
    if (!IsValid(CrossingGI))
    {
        UE_LOG(LogTemp, Warning, TEXT("[TargetCardChange] CrossingLevel subsystem is unavailable."));
        return false;
    }

    for (const auto& Pair : Runtime[CardSlot].EntryModifiers)
        if (ACoinActor* Coin = Pair.Key.Get(); IsValid(Coin) && Pair.Value.ExtraActions > 0)
            Coin->ClearAdditionalActions();
    ClearSlot(CardSlot, false);
    HandCards[CardSlot] = Card;
    bHasCard[CardSlot] = true;
    Runtime[CardSlot].CardID = CardID;
    CrossingGI->SetBattleCardID(CardID, CardSlot);
    CardPrice = 0;
    bool bHasPromotion = false;
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        if (!bHasCard[Slot]) continue;
        CardPrice += HandCards[Slot].Price;
        bHasPromotion |= HandCards[Slot].CardID == 3;
    }
    if (!bHasPromotion) ClearPromotionHighlight();

    if (bTurnInitialized)
    {
        if (CardID == 1 || CardID == 4)
        {
            TArray<FCoinOnGridInfo> FieldCoins;
            CollectCoinsOnField(FieldCoins);
            TMap<int32, int32> SourceSlots;
            BuildSourceSlots(SourceSlots);
            const bool bActive = FCardLogicLibrary::Evaluate(Card, FieldCoins, SourceSlots,
                BattleEntryGold, PromotionHighlightedGrid, DM, Runtime[CardSlot].EntryModifiers);
            SetCardActive(CardSlot, bActive);
            if (CardID == 1)
                for (const auto& Pair : Runtime[CardSlot].EntryModifiers)
                    if (ACoinActor* Coin = Pair.Key.Get(); IsValid(Coin)) Coin->GrantAdditionalActions(Pair.Value.ExtraActions);
        }
        if (CardID == 3 && PromotionHighlightedGrid.GridX < 0 && IsValid(GridSubsys) &&
            GridSubsys->GridXSize > 0 && GridSubsys->GridYSize > 0)
        {
            PromotionHighlightedGrid = FGridPoint(FMath::RandRange(0, GridSubsys->GridXSize - 1),
                FMath::RandRange(0, GridSubsys->GridYSize - 1));
            if (AGridActor* Grid = GridSubsys->GetGridActor(PromotionHighlightedGrid); IsValid(Grid) && IsValid(ActingManager))
                ActingManager->ShowPromotionVFX(Grid->GetActorLocation());
        }
        ExecuteCardsEffect();
    }
    OnHandCardSet.Broadcast(CardSlot, Card);
    OnBattleCardDataChanged.Broadcast();
    UE_LOG(LogTemp, Log, TEXT("[TargetCardChange] CardID=%d Slot=%d"), CardID, CardSlot);
    return true;
}

void UStageCardWSubsystem::RemoveHandCard(int32 HandIndex)
{
    if (HandIndex < 0 || HandIndex >= HandCount) return;

    UWorld* World = GetWorld();
    if (World)
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UCrossingLevelGISubsystem* LocalCrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>())
            {
                // �Լ� �ñ״�ó: SetBattleCardID(CardID, Slot)
                LocalCrossingGI->SetBattleCardID(-1, HandIndex);
            }
        }
    }

    ClearSlot(HandIndex, /*bNotify=*/true);
}

bool UStageCardWSubsystem::TryGetHandCard(int32 HandIndex, FCardData& Out) const
{
    if (HandIndex < 0 || HandIndex >= HandCount) return false;
    if (!bHasCard[HandIndex]) return false;

    Out = HandCards[HandIndex];
    return true;
}

void UStageCardWSubsystem::GetBattleCardSlots(TArray<FBattleCardSlotViewData>& OutCardSlots) const
{
    OutCardSlots.Reset();
    OutCardSlots.Reserve(HandCount);

    for (int32 SlotIndex = 0; SlotIndex < HandCount; ++SlotIndex)
    {
        FBattleCardSlotViewData& SlotView = OutCardSlots.AddDefaulted_GetRef();
        SlotView.SlotNumber = SlotIndex;
        SlotView.bOccupied = bHasCard.IsValidIndex(SlotIndex) && bHasCard[SlotIndex];
        if (SlotView.bOccupied && HandCards.IsValidIndex(SlotIndex))
        {
            SlotView.CardData = HandCards[SlotIndex];
        }

        SlotView.bIsActive = Runtime[SlotIndex].bActive;
    }
}

void UStageCardWSubsystem::ClearSlot(int32 HandIndex, bool bNotify)
{
    SyncCardModifiers(HandIndex, {});
    Runtime[HandIndex].EntryModifiers.Reset();
    SetCardActive(HandIndex, false);
    bHasCard[HandIndex] = false;
    HandCards[HandIndex] = FCardData();

    if (bNotify)
    {
        OnHandCardCleared.Broadcast(HandIndex);
        OnBattleCardDataChanged.Broadcast();
    }
}

int32 UStageCardWSubsystem::GetCardCount() const
{
    int32 CardCount = 0;
    for (bool bHas : bHasCard)
    {
        if (bHas)
        {
            CardCount++;
        }
    }

    return CardCount;
}

void UStageCardWSubsystem::ClearAllModifiers()
{
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        for (const auto& Pair : Runtime[Slot].EntryModifiers)
            if (ACoinActor* Coin = Pair.Key.Get(); IsValid(Coin) && Pair.Value.ExtraActions > 0)
                Coin->ClearAdditionalActions();
        SyncCardModifiers(Slot, {});
        Runtime[Slot].EntryModifiers.Reset();
    }
    UnActiveCardUI();
}

void UStageCardWSubsystem::SettingDoSettingPhase()
{
    bTurnInitialized = false;
    ClearAllModifiers();
    ClearPromotionHighlight();
}

void UStageCardWSubsystem::ClearPromotionHighlight()
{
    if (IsValid(ActingManager)) ActingManager->HidePromotionVFX();
    PromotionHighlightedGrid = FGridPoint(-1, -1);
}

FCoinCardModifiers UStageCardWSubsystem::GetModifiersForCoin(ACoinActor* Coin) const
{
    FCoinCardModifiers Result;
    if (!IsValid(Coin)) return Result;
    for (const FStageCardRuntimeState& State : Runtime)
    {
        if (const FCoinCardModifiers* Mods = State.Applied.Find(Coin))
        {
            Result.AttackAdd += Mods->AttackAdd;
            Result.BehaviorAdd += Mods->BehaviorAdd;
            Result.CountAdd += Mods->CountAdd;
            Result.RangeAdd += Mods->RangeAdd;
            Result.AbilityRangeAdd += Mods->AbilityRangeAdd;
            Result.ExtraActions += Mods->ExtraActions;
            Result.bLifeSteal |= Mods->bLifeSteal;
        }
    }
    return Result;
}

void UStageCardWSubsystem::CollectCoinsOnField(TArray<FCoinOnGridInfo>& OutCoins) const
{
    OutCoins.Reset();
    if (IsValid(GridSubsys)) GridSubsys->CollectOccupiedCoins(OutCoins);
    OutCoins.RemoveAll([](const FCoinOnGridInfo& Info)
    {
        return !IsValid(Info.CoinActor) || !IsValid(Info.CoinActor->StatComponent) || Info.CoinActor->StatComponent->IsDead();
    });
}

void UStageCardWSubsystem::BuildSourceSlots(TMap<int32, int32>& OutSlots) const
{
    OutSlots.Reset();
    const UCoinManagementWSubsystem* Coins = GetWorld() ? GetWorld()->GetSubsystem<UCoinManagementWSubsystem>() : nullptr;
    if (!IsValid(Coins)) return;
    for (const FReadyCoinData& Coin : Coins->GetReadyCoinData())
        if (Coin.CoinInstanceID != INDEX_NONE && Coin.SourceSlotNumber != INDEX_NONE)
            OutSlots.Add(Coin.CoinInstanceID, Coin.SourceSlotNumber);
}

void UStageCardWSubsystem::BeginCardTurn()
{
    if (bTurnInitialized) return;
    ClearAllModifiers();
    ClearPromotionHighlight();
    bTurnInitialized = true;
    TArray<FCoinOnGridInfo> FieldCoins;
    CollectCoinsOnField(FieldCoins);
    TMap<int32, int32> SourceSlots;
    BuildSourceSlots(SourceSlots);
    bool bHasPromotion = false;
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        if (!bHasCard[Slot]) continue;
        const FCardData& Card = HandCards[Slot];
        Runtime[Slot].CardID = Card.CardID;
        bHasPromotion |= Card.CardID == 3;
        if (Card.CardID == 1 || Card.CardID == 4)
        {
            const bool bActive = FCardLogicLibrary::Evaluate(Card, FieldCoins, SourceSlots,
                BattleEntryGold, PromotionHighlightedGrid, DM, Runtime[Slot].EntryModifiers);
            SetCardActive(Slot, bActive);
            if (Card.CardID == 1)
                for (const auto& Pair : Runtime[Slot].EntryModifiers)
                    if (ACoinActor* Coin = Pair.Key.Get(); IsValid(Coin)) Coin->GrantAdditionalActions(Pair.Value.ExtraActions);
        }
    }
    if (bHasPromotion && IsValid(GridSubsys) && GridSubsys->GridXSize > 0 && GridSubsys->GridYSize > 0)
    {
        PromotionHighlightedGrid = FGridPoint(FMath::RandRange(0, GridSubsys->GridXSize - 1),
            FMath::RandRange(0, GridSubsys->GridYSize - 1));
        if (AGridActor* Grid = GridSubsys->GetGridActor(PromotionHighlightedGrid); IsValid(Grid) && IsValid(ActingManager))
            ActingManager->ShowPromotionVFX(Grid->GetActorLocation());
    }
    ExecuteCardsEffect();
}

void UStageCardWSubsystem::Tick(float DeltaTime)
{
    ExecuteCardsEffect();
}

TStatId UStageCardWSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UStageCardWSubsystem, STATGROUP_Tickables);
}

void UStageCardWSubsystem::ExecuteCardsEffect()
{
    if (!bTurnInitialized || bRefreshingEffects || !IsValid(GetWorld())) return;
    const UBattleManagerWSubsystem* Battle = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (!IsValid(Battle)) return;
    const EPhaseState Phase = Battle->GetCurrentPhase();
    if (Phase != EPhaseState::CoinBehaviorPhase && Phase != EPhaseState::BossPhase) return;
    TGuardValue<bool> Guard(bRefreshingEffects, true);
    TArray<FCoinOnGridInfo> FieldCoins;
    CollectCoinsOnField(FieldCoins);
    TMap<int32, int32> SourceSlots;
    BuildSourceSlots(SourceSlots);
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        if (!bHasCard[Slot])
        {
            SyncCardModifiers(Slot, {});
            SetCardActive(Slot, false);
            continue;
        }
        const FCardData& Card = HandCards[Slot];
        if (Card.CardID == 1 || Card.CardID == 4)
        {
            // 진입 시 판정과 대상은 고정합니다. 사망한 액터는 Sync에서 제외합니다.
            SyncCardModifiers(Slot, Runtime[Slot].EntryModifiers);
        }
        else
        {
            TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> Desired;
            const bool bActive = FCardLogicLibrary::Evaluate(Card, FieldCoins, SourceSlots,
                BattleEntryGold, PromotionHighlightedGrid, DM, Desired);
            SyncCardModifiers(Slot, Desired);
            SetCardActive(Slot, bActive);
        }
    }
}

void UStageCardWSubsystem::SyncCardModifiers(int32 Slot, const TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Desired)
{
    FStageCardRuntimeState& State = Runtime[Slot];
    const int32 CardID = bHasCard.IsValidIndex(Slot) && bHasCard[Slot] ? HandCards[Slot].CardID : INDEX_NONE;
    const int32 BuffType = CardBuffTypeBase + Slot;
    for (auto It = State.Applied.CreateIterator(); It; ++It)
    {
        ACoinActor* Coin = It.Key().Get();
        if (State.CardID != CardID || !Desired.Contains(It.Key()) || !IsValid(Coin) ||
            !IsValid(Coin->StatComponent) || Coin->StatComponent->IsDead())
        {
            if (IsValid(Coin) && IsValid(Coin->StatComponent))
                Coin->StatComponent->RemoveStatusEffectsByTypeAndSource(BuffType, EStatusEffectSourceType::Card, State.CardID);
            It.RemoveCurrent();
        }
    }
    State.CardID = CardID;
    if (CardID == INDEX_NONE) return;
    for (const auto& Pair : Desired)
    {
        ACoinActor* Coin = Pair.Key.Get();
        if (!IsValid(Coin) || !IsValid(Coin->StatComponent) || Coin->StatComponent->IsDead()) continue;
        const FCoinCardModifiers* Previous = State.Applied.Find(Pair.Key);
        if (Previous && *Previous == Pair.Value &&
            Coin->StatComponent->GetStatusEffectStackCount(BuffType, EStatusEffectSourceType::Card, CardID) > 0) continue;
        FStatusEffectInstance Effect;
        Effect.BuffTypeID = BuffType;
        Effect.SourceType = EStatusEffectSourceType::Card;
        Effect.SourceDataID = CardID;
        Effect.DurationType = EBuffDurationType::TurnOnly;
        Effect.Modifier.AttackPoint = Pair.Value.AttackAdd;
        Effect.Modifier.WeaponPoint = Pair.Value.BehaviorAdd;
        Effect.Modifier.WeaponCnt = Pair.Value.CountAdd;
        Effect.Modifier.AttackRange = Pair.Value.RangeAdd;
        Effect.Modifier.AbilityRange = Pair.Value.AbilityRangeAdd;
        Effect.ReactiveBehavior = Pair.Value.bLifeSteal ? EStatusReactiveBehavior::LifeSteal : EStatusReactiveBehavior::None;
        if (Coin->StatComponent->SetCardStatusEffect(Effect)) State.Applied.Add(Pair.Key, Pair.Value);
    }
}

void UStageCardWSubsystem::SetCardActive(int32 Slot, bool bActive)
{
    if (Runtime[Slot].bActive == bActive) return;
    Runtime[Slot].bActive = bActive;
    OnStageHandCardActive.Broadcast(Slot, bActive);
    OnBattleCardDataChanged.Broadcast();
}

void UStageCardWSubsystem::UnActiveCardUI()
{
    for (int32 Slot = 0; Slot < HandCount; ++Slot) SetCardActive(Slot, false);
}
