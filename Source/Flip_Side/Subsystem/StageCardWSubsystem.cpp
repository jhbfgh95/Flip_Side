// StageCardWSubsystem.cpp
#include "Subsystem/StageCardWSubsystem.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"

#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Subsystem/CardLogicLibrary.h"
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

    FCardLogicLibrary::BuildLogicTable(CardLogicTable);
}

void UStageCardWSubsystem::Deinitialize()
{
    GridSubsys = nullptr;
    ActingManager = nullptr;
    CrossingGI = nullptr;
    DM = nullptr;
    CoinMods.Empty();

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

        // TODO: Tick 기반 조건 검사 이관 후 실제 활성 상태를 여기서 전달합니다.
        SlotView.bIsActive = false;
    }
}

void UStageCardWSubsystem::ClearSlot(int32 HandIndex, bool bNotify)
{
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
    CoinMods.Empty();
    UnActiveCardUI();
}

void UStageCardWSubsystem::SettingDoSettingPhase()
{
    ClearPromotionHighlight();
    //카드 활성화 초기화
    UnActiveCardUI();
}

void UStageCardWSubsystem::ClearPromotionHighlight()
{
    if (ActingManager)
    {
        ActingManager->HidePromotionVFX();
    }

    PromotionHighlightedGrid.GridX = -1;
    PromotionHighlightedGrid.GridY = -1;
}

FCoinCardModifiers UStageCardWSubsystem::GetModifiersForCoin(ACoinActor* Coin) const
{
    if (!IsValid(Coin))
        return FCoinCardModifiers{};

    if (const FCoinCardModifiers* Found = CoinMods.Find(Coin))
        return *Found;

    return FCoinCardModifiers{};
}

void UStageCardWSubsystem::CollectCoinsOnField(TArray<FCoinOnGridInfo>& OutCoins) const
{
    OutCoins.Reset();
    if (!GridSubsys) return;

    GridSubsys->CollectOccupiedCoins(OutCoins);
}


void UStageCardWSubsystem::ExecuteCardsEffect()
{
    ClearAllModifiers();
    ClearPromotionHighlight();

    TArray<FCoinOnGridInfo> FieldCoins;
    CollectCoinsOnField(FieldCoins);

    if (FieldCoins.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("[StageCard] ExecuteCardsEffect: No coins on field."));
        return;
    }

    // 필드 코인 턴 버프 초기화
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        if (!IsValid(Info.CoinActor)) continue;
        UComponent_Status* StatusComp = Info.CoinActor->FindComponentByClass<UComponent_Status>();
        if (IsValid(StatusComp)) StatusComp->ClearTurnBasedBuffs();
    }

    // 프로모션 카드 보유 여부 확인 -> 랜덤 그리드 빛내기
    bool bHasPromotion = false;
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        if (bHasCard.IsValidIndex(Slot) && bHasCard[Slot] && HandCards[Slot].CardID == 3)
        {
            bHasPromotion = true;
            break;
        }
    }

    if (bHasPromotion && GridSubsys && GridSubsys->GridXSize > 0 && GridSubsys->GridYSize > 0)
    {
        FGridPoint RandPoint;
        RandPoint.GridX = FMath::RandRange(0, GridSubsys->GridXSize - 1);
        RandPoint.GridY = FMath::RandRange(0, GridSubsys->GridYSize - 1);

        if (AGridActor* HighlightGrid = GridSubsys->GetGridActor(RandPoint))
        {
            if (ActingManager)
            {
                ActingManager->ShowPromotionVFX(HighlightGrid->GetActorLocation());
            }
            PromotionHighlightedGrid = RandPoint;
        }
    }

    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> LocalMods;

    // 핸드 카드마다 테이블에서 로직 조회 후 실행
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        const bool bHas = bHasCard.IsValidIndex(Slot) && bHasCard[Slot];
        UE_LOG(LogTemp, Warning, TEXT("[StageCard] Slot=%d, bHasCard=%d, CardID=%d"),
            Slot, bHas ? 1 : 0, bHas ? HandCards[Slot].CardID : -1);

        if (!bHas) continue;

        const FCardData& Card = HandCards[Slot];
        if (Card.CardID < 0) continue;

        if (Card.CardID == 3)
        {
            // 프로모션: 빛나는 칸에 쇠파이프(WeaponID==3) 코인이 있을 때만 버프
            if (PromotionHighlightedGrid.GridX >= 0)
            {
                if(FCardLogicLibrary::ApplyPromotion(Card, FieldCoins, LocalMods, DM, PromotionHighlightedGrid))
                    OnStageHandCardActive.Broadcast(Slot, true);
                else
                    OnStageHandCardActive.Broadcast(Slot, false);
            }
        }
        else if (const FCardLogicFn* Logic = CardLogicTable.Find(Card.CardID))
        {
            if((*Logic)(Card, FieldCoins, LocalMods, DM))
                OnStageHandCardActive.Broadcast(Slot, true);
            else
                OnStageHandCardActive.Broadcast(Slot, false);

        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[StageCard] CardID=%d 에 등록된 로직 없음"), Card.CardID);
            OnStageHandCardActive.Broadcast(Slot, false);
        }
    }

    // modifier를 Component_Status 버프로 등록
    for (const TPair<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Pair : LocalMods)
    {
        ACoinActor* Coin = Pair.Key.Get();
        if (!IsValid(Coin)) continue;

        UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
        if (!IsValid(StatusComp))
        {
            UE_LOG(LogTemp, Warning, TEXT("[StageCard] %s has no StatusComponent."), *GetNameSafe(Coin));
            continue;
        }

        const int32 AttackAdd   = Pair.Value.AttackAdd;
        const int32 BehaviorAdd = Pair.Value.BehaviorAdd;
        const int32 RangeAdd    = Pair.Value.RangeAdd;

        FBuffInfo BuffInfo;
        BuffInfo.BuffName = TEXT("StageCardBuff");
        BuffInfo.StatDelegate = FOnCalculateStats::FDelegate::CreateWeakLambda(
            StatusComp,
            [AttackAdd, BehaviorAdd, RangeAdd](FActionTask& Task)
            {
                Task.ModifiedAttackPoint   += AttackAdd;
                Task.ModifiedBehaviorPoint += BehaviorAdd;
                Task.ModifiedRange.GridX   += RangeAdd;
                Task.ModifiedRange.GridY   += RangeAdd;
            });

        StatusComp->AddBuffs(BuffInfo);
    }

    CoinMods = LocalMods;

    UE_LOG(LogTemp, Log, TEXT("[StageCard] ExecuteCardsEffect done. BuffAppliedCoins=%d"), LocalMods.Num());
}


void UStageCardWSubsystem::UnActiveCardUI()
{
    for(int i =0; i< HandCount; i++)
    {
        OnStageHandCardActive.Broadcast(i, false);
    }
}
