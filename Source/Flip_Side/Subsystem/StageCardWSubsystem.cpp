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
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Component_Status.h"
#include "CoinActor.h"
#include "GridActor.h"

#include "DataTypes/GridTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystem/LevelGISubsystem.h"

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

    const ULevelGISubsystem* Level = GI->GetSubsystem<ULevelGISubsystem>();
    const TArray<int32> IDs = IsValid(Level) && Level->IsBattleTutorialActive()
        ? TArray<int32>{3, 6, -1} : CrossingGI->GetBattleCardIDs();

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

    OnBattleCardDataChanged.Broadcast();
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
        if (SlotView.bOccupied && SlotView.CardData.CardID == 4 && SlotView.bIsActive)
        {
            if (const FCardGoldTierData* Tier = FCardLogicLibrary::FindGoldTier(SlotView.CardData, BattleEntryGold))
            {
                SlotView.AppliedModifiers = Tier->Modifiers;
                SlotView.bHasAppliedModifiers = true;
            }
        }
    }
}

void UStageCardWSubsystem::ClearSlot(int32 HandIndex, bool bNotify)
{
    SyncCardModifiers(HandIndex, {});
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
        SyncCardModifiers(Slot, {});
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
    if (bTurnInitialized || bEvaluationStopped) return;
    ClearAllModifiers();
    ClearPromotionHighlight();
    bTurnInitialized = true;
    bool bHasPromotion = false;
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
        bHasPromotion |= bHasCard[Slot] && HandCards[Slot].CardID == 3;
    const int32 CoinAreaHeight = IsValid(GridSubsys) ? GridSubsys->GetBossAreaStartY() : 0;
    if (bHasPromotion && IsValid(GridSubsys) && GridSubsys->GridXSize > 0 && CoinAreaHeight > 0)
    {
        PromotionHighlightedGrid = FGridPoint(FMath::RandRange(0, GridSubsys->GridXSize - 1),
            FMath::RandRange(0, CoinAreaHeight - 1));
        if (TutorialPromotionCell.GridX >= 0)
        {
            PromotionHighlightedGrid = TutorialPromotionCell;
            TutorialPromotionCell = FGridPoint(-1, -1);
        }
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
    if (bEvaluationStopped || bRefreshingEffects || !IsValid(GetWorld())) return;
    const UBattleManagerWSubsystem* Battle = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (!IsValid(Battle)) return;
    const EPhaseState Phase = Battle->GetCurrentPhase();
    if (Phase != EPhaseState::CoinReadyPhase && Phase != EPhaseState::CoinBehaviorPhase && Phase != EPhaseState::BossPhase) return;
    if (Phase != EPhaseState::CoinReadyPhase && !bTurnInitialized) return;
    TGuardValue<bool> Guard(bRefreshingEffects, true);
    TArray<FCoinOnGridInfo> FieldCoins;
    TMap<int32, int32> SourceSlots;
    if (bTurnInitialized)
    {
        CollectCoinsOnField(FieldCoins);
        BuildSourceSlots(SourceSlots);
    }
    bool bCardApplied = false;
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        if (!bHasCard[Slot])
        {
            SyncCardModifiers(Slot, {});
            SetCardActive(Slot, false);
            continue;
        }
        const FCardData& Card = HandCards[Slot];
        if (!bTurnInitialized && Card.bConditionRequiresFieldCoins)
        {
            SyncCardModifiers(Slot, {});
            SetCardActive(Slot, false);
            continue;
        }
        TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> Desired;
        const bool bActive = FCardLogicLibrary::Evaluate(Card, FieldCoins, SourceSlots,
            BattleEntryGold, PromotionHighlightedGrid, DM, Desired);
        bCardApplied |= SyncCardModifiers(Slot, Desired);
        SetCardActive(Slot, bActive);
    }

    // 새로 효과를 준 카드가 있으면 여러 장이어도 소리는 한 번만 냅니다. 보스 차례에 켜진 카드는 다음 턴 전에 사라지므로 소리를 내지 않습니다.
    if (bCardApplied && Phase == EPhaseState::CoinBehaviorPhase)
    {
        if (USoundManagerWSubsystem* SoundManager = GetWorld()->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayCardActivateSound();
        }
    }
}

bool UStageCardWSubsystem::SyncCardModifiers(int32 Slot, const TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Desired)
{
    FStageCardRuntimeState& State = Runtime[Slot];
    const int32 CardID = bHasCard.IsValidIndex(Slot) && bHasCard[Slot] ? HandCards[Slot].CardID : INDEX_NONE;
    const int32 BuffType = CardBuffTypeBase + Slot;
    const bool bGoldCard = State.CardID == 4 || CardID == 4;
    bool bAppliedChanged = false;
    bool bNewlyApplied = false;
    for (auto It = State.Applied.CreateIterator(); It; ++It)
    {
        ACoinActor* Coin = It.Key().Get();
        if (State.CardID != CardID || !Desired.Contains(It.Key()) || !IsValid(Coin) ||
            !IsValid(Coin->StatComponent) || Coin->StatComponent->IsDead())
        {
            if (IsValid(Coin))
            {
                Coin->AdjustAdditionalActionLimit(-FMath::Max(0, It.Value().ExtraActions));
                if (IsValid(Coin->StatComponent))
                    Coin->StatComponent->RemoveStatusEffectsByTypeAndSource(BuffType, EStatusEffectSourceType::Card, State.CardID);
            }
            It.RemoveCurrent();
            bAppliedChanged = true;
        }
    }
    State.CardID = CardID;
    if (CardID == INDEX_NONE)
    {
        if (bGoldCard && bAppliedChanged) OnBattleCardDataChanged.Broadcast();
        return false;
    }
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
        const bool bModifiersChanged = !Previous || !(*Previous == Pair.Value);
        const bool bFirstApply = !Previous;
        const int32 PreviousExtraActions = Previous ? FMath::Max(0, Previous->ExtraActions) : 0;
        if (Coin->StatComponent->SetCardStatusEffect(Effect))
        {
            State.Applied.Add(Pair.Key, Pair.Value);
            Coin->AdjustAdditionalActionLimit(FMath::Max(0, Pair.Value.ExtraActions) - PreviousExtraActions);
            bAppliedChanged |= bModifiersChanged;
            bNewlyApplied |= bFirstApply;
        }
    }
    // 실제 적용/해제 이후에도 알림을 보내 호버 중인 추가 정보가 이전 턴의 값을 남기지 않게 합니다.
    if (bGoldCard && bAppliedChanged) OnBattleCardDataChanged.Broadcast();
    return bNewlyApplied;
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
