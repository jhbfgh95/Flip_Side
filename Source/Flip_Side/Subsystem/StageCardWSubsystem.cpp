// StageCardWSubsystem.cpp
#include "Subsystem/StageCardWSubsystem.h"

#include "FlipSideDevloperSettings.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
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

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (Settings && !Settings->StageHUDWidgetClass.IsNull())
    {
        StageHUDClass = Settings->StageHUDWidgetClass.LoadSynchronous();
    }

    FCardLogicLibrary::BuildLogicTable(CardLogicTable);
}

void UStageCardWSubsystem::Deinitialize()
{
    StageHUDInstance = nullptr;
    GridSubsys = nullptr;
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

    if (UGameInstance* GI = InWorld.GetGameInstance())
    {
        CrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>();
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
    }

    // ===== HUD ���� + ���� ����ȭ =====
    EnsureStageHUD(InWorld);
    RefreshHandFromGI();

    // ===== ���� ���� Ÿ�̹� ī�� ó��=====
    ClearAllModifiers();
}

void UStageCardWSubsystem::EnsureStageHUD(UWorld& InWorld)
{
    if (StageHUDInstance) return;

    if (!StageHUDClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] StageHUDClass is null. Set it in defaults/BP."));
        return;
    }

    APlayerController* PC = InWorld.GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] No PlayerController yet."));
        return;
    }

    StageHUDInstance = CreateWidget<UUserWidget>(PC, StageHUDClass);
    if (!StageHUDInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] CreateWidget failed."));
        return;
    }

    StageHUDInstance->AddToViewport();
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

    const TArray<int32> IDs = CrossingGI->GetBattleCardIDs();

    auto IsEmpty = [&]()
        {
            if (IDs.Num() < HandCount) return true;
            return (IDs[0] < 0 && IDs[1] < 0 && IDs[2] < 0);
        };

    // �׽�Ʈ�� �ڵ� ä��
    if (IsEmpty())
    {
        CrossingGI->SetBattleCardID(3, 0);
        CrossingGI->SetBattleCardID(5, 1);
        CrossingGI->SetBattleCardID(6, 2);

        UE_LOG(LogTemp, Warning, TEXT("[StageCard] BattleCardIDs were empty. Filled with 1,2,3 for test."));
    }

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

        OnHandCardSet.Broadcast(Slot, CardData);
    }
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

void UStageCardWSubsystem::ClearSlot(int32 HandIndex, bool bNotify)
{
    bHasCard[HandIndex] = false;
    HandCards[HandIndex] = FCardData();

    if (bNotify)
    {
        OnHandCardCleared.Broadcast(HandIndex);
    }
}

void UStageCardWSubsystem::ClearAllModifiers()
{
    CoinMods.Empty();
}

void UStageCardWSubsystem::ClearPromotionHighlight()
{
    if (GridSubsys && PromotionHighlightedGrid.GridX >= 0)
    {
        if (AGridActor* PrevGrid = GridSubsys->GetGridActor(PromotionHighlightedGrid))
        {
            PrevGrid->SetPromotionHighlight(false);
        }
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
            HighlightGrid->SetPromotionHighlight(true);
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
                FCardLogicLibrary::ApplyPromotion(Card, FieldCoins, LocalMods, DM, PromotionHighlightedGrid);
            }
        }
        else if (const FCardLogicFn* Logic = CardLogicTable.Find(Card.CardID))
        {
            (*Logic)(Card, FieldCoins, LocalMods, DM);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[StageCard] CardID=%d 에 등록된 로직 없음"), Card.CardID);
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