// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystems/Subsystem.h"
#include "FlipSideDevloperSettings.h"
#include "DataManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "CoinActionManagementWSubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "UseableItemActor.h"
#include "FlipSide_Enum.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "BattlePlayerController_FlipSide.h"
#include "ItemDataTypes.h"
#include "GridTypes.h"
#include "Item_Action.h"

namespace
{
    constexpr int32 PhaseChangePotionItemID = 4;
    constexpr int32 EverywherePotionItemID = 6;
}

void UUseableItemWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitUseitemSlot();

    SelectedItemAction = NewObject<UItem_Action>(this);

    CoinActionManager = Collection.InitializeDependency<UCoinActionManagementWSubsystem>();
    CoinManager = Collection.InitializeDependency<UCoinManagementWSubsystem>();
    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
    ActingManager = Collection.InitializeDependency<UBattleLevelActingWSubsystem>();

    if(SelectedItemAction)
    {
        InitSelectedItem();
        //SingleCell 기본 물어보기.
        ItemAreaSpec = {
            EAttackAreaPattern::SingleCell
        };
    }
}

void UUseableItemWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if(InWorld.IsGameWorld())
    {
        InitializeBattleItemSlots();
    }

    if(GridManager)
    {
        GridManager->OnGridClickedForItem.BindDynamic(this, &UUseableItemWSubsystem::ExecuteItemForGrid);
    }
}

bool UUseableItemWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if(!Super::ShouldCreateSubsystem(Outer)) return false;

    UWorld* World = Cast<UWorld>(Outer);

    if(World)
    {
        FString MapName = World->GetName();
        if(MapName.Contains(TEXT("L_Stage")))
        {
            return true;
        }
    }

    return false;
}

void UUseableItemWSubsystem::InitUseitemSlot()
{
    UWorld* World = GetWorld();
    if(!IsValid(World)) return;

    UGameInstance* GI = World->GetGameInstance();
    if(!IsValid(GI)) return;

    UCrossingLevelGISubsystem* CrossingLevelSubsystem = GI->GetSubsystem<UCrossingLevelGISubsystem>();
    if(!CrossingLevelSubsystem) return;

    ItemSlotArray.Reset();
    for(int i = 0; i < CrossingLevelSubsystem->GetMakedItemNum(); i++)
    {
        FSelectItem ItemData = CrossingLevelSubsystem->GetBattleUseItems(i);
        ItemSlotArray.Add(ItemData);
    }
}

void UUseableItemWSubsystem::InitializeBattleItemSlots()
{
    UWorld* World = GetWorld();
    if(!IsValid(World)) return;

    UGameInstance* GI = World->GetGameInstance();
    if(!IsValid(GI)) return;

    UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();
    if(!IsValid(DM) || !DM->IsCacheReady()) return;

    BattleItemSlots.Reset();
    for(const FSelectItem& SelectItemData : ItemSlotArray)
    {
        if(SelectItemData.SameItemNum <= 0) continue;

        FItemData ItemData;
        if(!DM->TryGetItem(SelectItemData.ItemID, ItemData)) continue;

        FBattleItemSlotData* ExistingSlot = FindBattleItemSlot(ItemData.ItemID);
        if(ExistingSlot)
        {
            ExistingSlot->AvailableCount += SelectItemData.SameItemNum;
            continue;
        }

        FBattleItemSlotData& NewSlot = BattleItemSlots.AddDefaulted_GetRef();
        NewSlot.ItemData = ItemData;
        NewSlot.AvailableCount = SelectItemData.SameItemNum;
    }

    OnBattleItemDataChanged.Broadcast();
}

FBattleItemSlotData* UUseableItemWSubsystem::FindBattleItemSlot(int32 ItemID)
{
    return BattleItemSlots.FindByPredicate([ItemID](const FBattleItemSlotData& ItemSlot)
    {
        return ItemSlot.ItemData.ItemID == ItemID;
    });
}

const FBattleItemSlotData* UUseableItemWSubsystem::FindBattleItemSlot(int32 ItemID) const
{
    return BattleItemSlots.FindByPredicate([ItemID](const FBattleItemSlotData& ItemSlot)
    {
        return ItemSlot.ItemData.ItemID == ItemID;
    });
}

void UUseableItemWSubsystem::ConsumeSelectedItem()
{
    ConsumeSelectedItemOnly();
}

void UUseableItemWSubsystem::ConsumeSelectedItemOnly()
{
    StopItemCursorPreview();

    FBattleItemSlotData* SelectedItemSlot = FindBattleItemSlot(SelectedItemID);
    if(SelectedItemSlot && SelectedItemSlot->AvailableCount > 0)
    {
        --SelectedItemSlot->AvailableCount;
        OnBattleItemDataChanged.Broadcast();
    }

    // EverywherePotion은 코인 선택 단계에서 먼저 소모한 뒤 그리드 선택을 이어갑니다.
    SelectedItemID = INDEX_NONE;
}

void UUseableItemWSubsystem::PlayItemFailedFeedback()
{
    if(ActingManager && IsValid(PreviewItemActor))
    {
        ActingManager->PlayUseableItemFailedVFX(PreviewItemActor);
    }

    //SoundManager에 아이템 실패 사운드 API가 추가되면 여기서 호출.
}

void UUseableItemWSubsystem::PlaySelectedItemSuccessVFX(AGridActor* TargetGrid, ACoinActor* TargetCoin, AActor* TargetOther)
{
    if(!ActingManager || !bHasSelectedItemData) return;

    ActingManager->PlayUseableItemVFX(SelectedItemData, TargetGrid, TargetCoin, TargetOther);

    if(SelectedItemData.ItemID == PhaseChangePotionItemID)
    {
        ActingManager->PlayPhaseChangePotionAct(TargetCoin);
    }

    //SoundManager에 아이템 성공 사운드 API가 추가되면 여기서 호출.
}

void UUseableItemWSubsystem::StartItemCursorPreview(const FItemData& SourceItemData)
{
    StopItemCursorPreview();

    UWorld* World = GetWorld();
    if(!World) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    UClass* ItemVisualTemplateClass = Settings ? Settings->UseableItemActor.LoadSynchronous() : nullptr;
    if(!ItemVisualTemplateClass || !ItemVisualTemplateClass->IsChildOf(AUseableItemActor::StaticClass())) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    PreviewItemActor = World->SpawnActor<AUseableItemActor>(
        ItemVisualTemplateClass,
        FTransform::Identity,
        SpawnParams
    );

    if(!IsValid(PreviewItemActor)) return;

    PreviewItemActor->InitializeAsCursorPreview(SourceItemData, ItemPreviewScaleMultiplier);
    UpdateItemCursorPreview();

    World->GetTimerManager().SetTimer(
        ItemPreviewFollowTimerHandle,
        this,
        &UUseableItemWSubsystem::UpdateItemCursorPreview,
        0.01f,
        true
    );
}

void UUseableItemWSubsystem::UpdateItemCursorPreview()
{
    if(!IsValid(PreviewItemActor))
    {
        StopItemCursorPreview();
        return;
    }

    UWorld* World = GetWorld();
    if(!World) return;

    ABattlePlayerController_FlipSide* BattlePC = Cast<ABattlePlayerController_FlipSide>(World->GetFirstPlayerController());
    if(!BattlePC) return;

    FVector CursorWorldLocation;
    if(BattlePC->GetCursorWorldLocationOnPlane(ItemPreviewPlaneZ, CursorWorldLocation))
    {
        PreviewItemActor->SetActorLocation(CursorWorldLocation);
    }
}

void UUseableItemWSubsystem::StopItemCursorPreview()
{
    UWorld* World = GetWorld();
    if(World)
    {
        World->GetTimerManager().ClearTimer(ItemPreviewFollowTimerHandle);
    }

    if(IsValid(PreviewItemActor))
    {
        PreviewItemActor->Destroy();
    }
    PreviewItemActor = nullptr;
}

void UUseableItemWSubsystem::InitSelectedItem()
{
    if(!IsValid(SelectedItemAction))
    {
        return;
    }

    SelectedItemAction->SetActionRange(DefaultItemRange);
    SelectedItemAction->SetLogicID(-1);
    SelectedItemAction->SetTargetGrid(nullptr);
    SelectedItemAction->SetItemEffectValue(-1);
    SelectedItemAction->SetInRangeCoins(nullptr);
    SelectedItemAction->SetInRangeOthers(nullptr);
    SelectedItemAction->SetInRangeBoss(nullptr);
    CurrentTargetMode = EUseableItemTargetMode::None;
    SelectedItemID = INDEX_NONE;
    SelectedItemData = FItemData();
    bHasSelectedItemData = false;
    SelectedTargetCoin = nullptr;
    ValidTargetGrids.Empty();
    if(CoinManager)
    {
        CoinManager->SetBattleCoinItemFlags(false);
    }
    if(GridManager)
    {
        GridManager->SetGridClickFlag(EGridClickFlag::None);
        GridManager->SetGridItemFlags(0);
    }
}

bool UUseableItemWSubsystem::SetItemInfo(int32 ItemID)
{
    if(!bIsCoinBehaviorTurn) return false;

    // SelectedItemAction이 혹시라도 Null인지 체크 // 크래시 발생해서 추가함
    if (!SelectedItemAction)
    {
        SelectedItemAction = NewObject<UItem_Action>(this);
        if (!SelectedItemAction) return false;
    }

    const FBattleItemSlotData* ItemSlot = FindBattleItemSlot(ItemID);
    if(!ItemSlot || ItemSlot->AvailableCount <= 0) return false;

    const FItemData& SelectItem = ItemSlot->ItemData;
    SelectedItemAction->SetLogicID(SelectItem.ItemID);
    SelectedItemAction->SetItemEffectValue(SelectItem.ItemEffectValue);
    FGridPoint ItemRange{SelectItem.ItemRange, SelectItem.ItemRange};
    SelectedItemAction->SetActionRange(ItemRange);
    return true;
}

//나중에 Spec나오면 이거 부르면 될듯함
//음..근데 그리드 기준으로 spec검사를 해야하긴 하는데
void UUseableItemWSubsystem::ApplyRangedThings()
{
    if(IsValid(GridManager) && IsValid(SelectedItemAction))
    {
        FObjectOnGridInfo GridInfos;

        GridManager->GetObjectsAtRange(ItemAreaSpec, SelectedItemAction->GetActionRange(), OutCells, GridInfos);
        for(AActor* Actor : GridInfos.Coins)
        {
            if(ACoinActor* Coin = Cast<ACoinActor>(Actor))
            {
                SelectedItemAction->SetInRangeCoins(Coin);
            }
        }
    }
}

void UUseableItemWSubsystem::CancelWantUseItem()
{
    StopItemCursorPreview();
    InitSelectedItem();
    if(CoinActionManager)
    {
        CoinActionManager->SetTurn(bIsCoinBehaviorTurn);
    }
}

void UUseableItemWSubsystem::BuildEverywhereValidTargetGrids(ACoinActor* TargetCoin)
{
    ValidTargetGrids.Empty();

    if(!TargetCoin || !GridManager) return;

    const FGridPoint CoinGrid = TargetCoin->GetDecidedGrid();
    const TArray<FGridPoint> CandidateGrids = {
        FGridPoint{CoinGrid.GridX, CoinGrid.GridY + 1},
        FGridPoint{CoinGrid.GridX, CoinGrid.GridY - 1},
        FGridPoint{CoinGrid.GridX - 1, CoinGrid.GridY},
        FGridPoint{CoinGrid.GridX + 1, CoinGrid.GridY}
    };

    for(const FGridPoint& Candidate : CandidateGrids)
    {
        AGridActor* Grid = GridManager->GetGridActor(Candidate);
        if(!Grid) continue;
        if(Grid->GetIsOccupied()) continue;

        ValidTargetGrids.Add(Candidate);
    }
}

void UUseableItemWSubsystem::ExecuteItemForGrid(AGridActor* TargetGrid)
{
    if(CurrentTargetMode != EUseableItemTargetMode::Grid)
    {
        PlayItemFailedFeedback();
        return;
    }

    if(!IsValid(TargetGrid) || TargetGrid->GetIsOccupied() || TargetGrid->GetItemFlag() == 0)
    {
        PlayItemFailedFeedback();
        return;
    }

    if(IsValid(SelectedTargetCoin) && !ValidTargetGrids.Contains(TargetGrid->GetGridPoint()))
    {
        PlayItemFailedFeedback();
        return;
    }

    if(IsValid(SelectedItemAction))
    {
        SelectedItemAction->SetTargetGrid(TargetGrid);
        SelectedItemAction->ExecuteAction();
        PlaySelectedItemSuccessVFX(TargetGrid, SelectedTargetCoin, nullptr);
        ConsumeSelectedItem();
    }
    CancelWantUseItem();
}

void UUseableItemWSubsystem::ExecuteItemForCoin(ACoinActor* TargetCoin)
{
    if(CurrentTargetMode == EUseableItemTargetMode::CoinThenGrid)
    {
        if(!IsValid(TargetCoin) || !IsValid(SelectedItemAction) || SelectedItemID == INDEX_NONE)
        {
            PlayItemFailedFeedback();
            return;
        }

        SelectedTargetCoin = TargetCoin;
        SelectedItemAction->SetInRangeCoins(TargetCoin);
        if(GridManager)
        {
            SelectedItemAction->SetTargetGrid(GridManager->GetGridActor(TargetCoin->GetDecidedGrid()));
        }

        BuildEverywhereValidTargetGrids(TargetCoin);
        if(ValidTargetGrids.IsEmpty())
        {
            PlayItemFailedFeedback();
            return;
        }

        ConsumeSelectedItemOnly();
        CurrentTargetMode = EUseableItemTargetMode::Grid;
        if(CoinManager)
        {
            CoinManager->SetBattleCoinItemFlags(false);
        }
        if(GridManager)
        {
            GridManager->SetGridItemTargetGrids(ValidTargetGrids);
        }
        return;
    }

    if(CurrentTargetMode != EUseableItemTargetMode::Coin)
    {
        PlayItemFailedFeedback();
        return;
    }

    if(!IsValid(TargetCoin) || !IsValid(SelectedItemAction))
    {
        PlayItemFailedFeedback();
        return;
    }

    if(IsValid(TargetCoin) && IsValid(SelectedItemAction))
    {
        SelectedItemAction->SetInRangeCoins(TargetCoin);
        if(GridManager)
        {
            SelectedItemAction->SetTargetGrid(GridManager->GetGridActor(TargetCoin->GetDecidedGrid()));
        }
        SelectedItemAction->ExecuteAction();
        PlaySelectedItemSuccessVFX(nullptr, TargetCoin, nullptr);
        ConsumeSelectedItem();
    }
    CancelWantUseItem();
}
bool UUseableItemWSubsystem::IsItemUseAvailable() const
{
    return bIsCoinBehaviorTurn &&
        (!IsValid(CoinActionManager) || !CoinActionManager->IsActionSequenceActive());
}

bool UUseableItemWSubsystem::TrySelectItem(int32 ItemID)
{
    if(!IsItemUseAvailable())
    {
        return false;
    }

    const FBattleItemSlotData* ItemSlot = FindBattleItemSlot(ItemID);
    if(!ItemSlot || ItemSlot->AvailableCount <= 0)
    {
        return false;
    }

    const FItemData ItemData = ItemSlot->ItemData;
    if(ItemData.ItemType != EItemType::CoinBuff && ItemData.ItemType != EItemType::Install)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UseableItem] Unsupported item type. ItemID=%d"), ItemID);
        return false;
    }

    if(CoinActionManager)
    {
        CoinActionManager->CancelSelectWeapon();
    }

    CancelWantUseItem();

    if(CoinActionManager)
    {
        CoinActionManager->SetTurn(false);
    }

    if(!SetItemInfo(ItemID))
    {
        CancelWantUseItem();
        return false;
    }

    CurrentTargetMode = ItemData.ItemType == EItemType::Install
        ? EUseableItemTargetMode::Grid
        : (ItemID == EverywherePotionItemID ? EUseableItemTargetMode::CoinThenGrid : EUseableItemTargetMode::Coin);
    SelectedItemID = ItemID;
    SelectedItemData = ItemData;
    bHasSelectedItemData = true;
    StartItemCursorPreview(SelectedItemData);

    if(CoinManager)
    {
        CoinManager->SetBattleCoinItemFlags(true);
    }

    if(GridManager)
    {
        if(CurrentTargetMode == EUseableItemTargetMode::Grid)
        {
            GridManager->SetGridClickFlag(EGridClickFlag::ItemAction);
            GridManager->SetGridItemFlags(1);
        }
        else
        {
            GridManager->SetGridClickFlag(EGridClickFlag::None, false);
            GridManager->SetGridHoverFlags(2);
        }
    }

    return true;
}

void UUseableItemWSubsystem::CoinBindsToItemMan()
{
    if(!CoinManager) return;
    TArray<ACoinActor*> Coins = CoinManager->GetReadyCoins();

    for(ACoinActor* Coin : Coins)
    {
        if(IsValid(Coin))
        {
            Coin->OnCoinClickForItemExcute.AddUniqueDynamic(this, &UUseableItemWSubsystem::ExecuteItemForCoin);
        }
    }
}

void UUseableItemWSubsystem::SetTurn(const bool bIsTurn)
{
    bIsCoinBehaviorTurn = bIsTurn;

    if(!bIsCoinBehaviorTurn)
    {
        CancelWantUseItem();
    }

    OnBattleItemDataChanged.Broadcast();
}

int32 UUseableItemWSubsystem::CalculateItemPrice() const
{
    int32 ReturnItemPrice = 0;
    for(const FBattleItemSlotData& ItemSlot : BattleItemSlots)
    {
        ReturnItemPrice += ItemSlot.ItemData.Price * FMath::Max(0, ItemSlot.AvailableCount);
    }

    return static_cast<int32>(ReturnItemPrice / 2);
}

int32 UUseableItemWSubsystem::CalculateItemCount() const
{
    int32 ItemCount = 0;
    for (const FBattleItemSlotData& ItemSlot : BattleItemSlots)
    {
        ItemCount += FMath::Max(0, ItemSlot.AvailableCount);
    }

    return ItemCount;
}
