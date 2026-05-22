// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystems/Subsystem.h"
#include "FlipSideDevloperSettings.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DataManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "CoinActionManagementWSubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "UseableItemActor.h"
#include "FlipSide_Enum.h"
#include "SlotActor.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "ItemPreviewActor.h"
#include "BattlePlayerController_FlipSide.h"
#include "ItemDataTypes.h"
#include "GridTypes.h"
#include "Item_Action.h"
#include "W_ItemInfo.h"

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
        InstanceUseItems();
        if(!ItemInfoWidgetInstance)
        {
            const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
            if(Settings && !Settings->ItemHoverWidget.IsNull())
            {
                UClass* ItemInfoWidgetClass = Settings->ItemHoverWidget.LoadSynchronous();

                if(ItemInfoWidgetClass && ItemInfoWidgetClass->IsChildOf(UW_ItemInfo::StaticClass()))
                {
                    ItemInfoWidgetInstance = CreateWidget<UW_ItemInfo>(GetWorld(), ItemInfoWidgetClass);
                    if(ItemInfoWidgetInstance)
                    {
                        ItemInfoWidgetInstance->AddToViewport();
                        ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
                    }
                }
            }
        }
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
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if(!GI) return;

    UCrossingLevelGISubsystem* CrossingLevelSubsystem = GI->GetSubsystem<UCrossingLevelGISubsystem>();
    if(!CrossingLevelSubsystem) return;

    for(int i = 0; i < CrossingLevelSubsystem->GetMakedItemNum(); i++)
    {
        FSelectItem ItemData = CrossingLevelSubsystem->GetBattleUseItems(i);
        ItemSlotArray.Add(ItemData);
    }
}

void UUseableItemWSubsystem::InstanceUseItems()
{
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if(!GI) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if(!Settings) return;

    UClass* BPUseItem = Settings->UseableItemActor.LoadSynchronous();
    UClass* BPItemSlot = Settings->UseableItemSlotActor.LoadSynchronous();
    if(!BPUseItem || !BPItemSlot) return;

    TArray<AActor*> OutSlots;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), BPItemSlot, OutSlots);
    OutSlots.Sort([](const AActor& A, const AActor& B){
        const ASlotActor* SlotA = Cast<ASlotActor>(&A);
        const ASlotActor* SlotB = Cast<ASlotActor>(&B);

        if(SlotA && SlotB)
        {
            return SlotA->GetSlotIndex() < SlotB->GetSlotIndex();
        }
        return false;
    });

    UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();

    if(DM && DM->IsCacheReady() && BPUseItem->IsChildOf(AUseableItemActor::StaticClass()))
    {
        int32 UseItemSlotIndex = 0;

        for(const FSelectItem& SelectItemData : ItemSlotArray)
        {
            FItemData ItemData;
            if(DM->TryGetItem(SelectItemData.ItemID, ItemData))
            {
                for(int i = 0; i < SelectItemData.SameItemNum; i++)
                {
                    if(OutSlots.IsValidIndex(UseItemSlotIndex))
                    {
                        ASlotActor* TargetSlot = Cast<ASlotActor>(OutSlots[UseItemSlotIndex]);
                        if(TargetSlot)
                        {
                            FTransform SpawnTransform = TargetSlot->GetSlotTransform();
                            if(i > 0)
                            {
                                FVector NewLocation = SpawnTransform.GetLocation() + FVector(0.f , 0.f , i * 35.f);
                                SpawnTransform.SetLocation(NewLocation);
                            }

                            AUseableItemActor* NewItem = GetWorld()->SpawnActorDeferred<AUseableItemActor>(
                                BPUseItem,
                                SpawnTransform
                            );
                            
                            if(NewItem)
                            {
                                NewItem->SetItemValues(ItemData.ItemID, ItemData.ItemType,ItemData.ItemIcon, ItemData.TypeColor, ItemData.Price);
                                NewItem->FinishSpawning(SpawnTransform);
                                BindItemDelegates(NewItem);
                                UnUsedItemArray.Add(NewItem);
                            }
                        }
                    }

                }
                UseItemSlotIndex++;
            }
        }
    }
}

void UUseableItemWSubsystem::BindItemDelegates(AUseableItemActor* TargetItem)
{
    if(TargetItem)
    {
        TargetItem->OnHoverItem.AddDynamic(this, &UUseableItemWSubsystem::VisibleItemInfoUI);
        TargetItem->OnUnhoverItem.AddDynamic(this, &UUseableItemWSubsystem::HideItemInfoUi);
        //걍 여기서 함수 부르면 되는거 아닌가..
        //클릭 다른 곳 하면 그때 Init불러도 늦지 않으니..
        TargetItem->OnGridClickItem.AddDynamic(this, &UUseableItemWSubsystem::SelectWantUseGridItem);
        TargetItem->OnCoinClickItem.AddDynamic(this, &UUseableItemWSubsystem::SelectWantUseCoinItem);
        TargetItem->OnItemRightClick.AddDynamic(this, &UUseableItemWSubsystem::HandleItemRightClicked);
    }
}

bool UUseableItemWSubsystem::TryGetItemData(AUseableItemActor* TargetItem, FItemData& OutItemData)
{
    if(!TargetItem) return false;

    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if(!GI) return false;

    UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();
    return DM && DM->TryGetItem(TargetItem->GetItemID(), OutItemData);
}

void UUseableItemWSubsystem::ConsumeSelectedItemActor()
{
    ConsumeSelectedItemActorOnly();
}

void UUseableItemWSubsystem::ConsumeSelectedItemActorOnly()
{
    StopItemCursorPreview();

    if(ItemInfoWidgetInstance)
    {
        ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }

    if(IsValid(SelectedItemActor))
    {
        SelectedItemActor->Destroy();
    }
    SelectedItemActor = nullptr;
}

void UUseableItemWSubsystem::PlayItemFailedFeedback()
{
    if(ActingManager && IsValid(ItemPreviewActor))
    {
        ActingManager->PlayUseableItemFailedVFXAtActor(ItemPreviewActor);
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

void UUseableItemWSubsystem::StartItemCursorPreview(AUseableItemActor* SourceItem)
{
    StopItemCursorPreview();

    UWorld* World = GetWorld();
    if(!World || !IsValid(SourceItem)) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ItemPreviewActor = World->SpawnActor<AItemPreviewActor>(
        AItemPreviewActor::StaticClass(),
        SourceItem->GetActorTransform(),
        SpawnParams
    );

    if(!ItemPreviewActor) return;

    ItemPreviewActor->InitFromItem(SourceItem);
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
    if(!IsValid(ItemPreviewActor))
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
        ItemPreviewActor->SetActorLocation(CursorWorldLocation);
    }
}

void UUseableItemWSubsystem::StopItemCursorPreview()
{
    UWorld* World = GetWorld();
    if(World)
    {
        World->GetTimerManager().ClearTimer(ItemPreviewFollowTimerHandle);
    }

    if(IsValid(ItemPreviewActor))
    {
        ItemPreviewActor->Destroy();
    }
    ItemPreviewActor = nullptr;
}

void UUseableItemWSubsystem::InitSelectedItem()
{
    SelectedItemAction->SetActionRange(DefaultItemRange);
    SelectedItemAction->SetLogicID(-1);
    SelectedItemAction->SetTargetGrid(nullptr);
    SelectedItemAction->SetItemEffectValue(-1);
    SelectedItemAction->SetInRangeCoins(nullptr);
    SelectedItemAction->SetInRangeOthers(nullptr);
    SelectedItemAction->SetInRangeBoss(nullptr);
    CurrentTargetMode = EUseableItemTargetMode::None;
    SelectedItemActor = nullptr;
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

void UUseableItemWSubsystem::SetItemInfo(AUseableItemActor* TargetItem)
{
    if(!bIsCoinSelectTurn) return;

    if(!TargetItem) return;

    // SelectedItemAction이 혹시라도 Null인지 체크 // 크래시 발생해서 추가함
    if (!SelectedItemAction)
    {
        SelectedItemAction = NewObject<UItem_Action>(this);
        if (!SelectedItemAction) return;
    }

    FItemData SelectItem;
    FGridPoint ItemRange;

    if(TryGetItemData(TargetItem, SelectItem) && SelectedItemAction)
    {
        SelectedItemAction->SetLogicID(TargetItem->GetItemID());
        //SelectedItemAction->SetItemType(SelectItem.ItemType);
        SelectedItemAction->SetItemEffectValue(SelectItem.ItemEffectValue);
        //ItemAreaSpec = SeletItem.ItemSpec;
        //이거 왜 int임?
        SelectedItemAction->SetActionRange(ItemRange = {SelectItem.ItemRange, SelectItem.ItemRange});
    }
}

//나중에 Spec나오면 이거 부르면 될듯함
//음..근데 그리드 기준으로 spec검사를 해야하긴 하는데
void UUseableItemWSubsystem::ApplyRangedThings()
{
    if(GridManager)
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
    if(ItemInfoWidgetInstance)
    {
        ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
    if(CoinActionManager)
    {
        CoinActionManager->HideCoinInfoForItemTarget();
    }
    InitSelectedItem();
    if(CoinActionManager)
    {
        CoinActionManager->SetTurn(bIsCoinSelectTurn);
    }
}

bool UUseableItemWSubsystem::HasPendingItemSelection() const
{
    return CurrentTargetMode != EUseableItemTargetMode::None
        || IsValid(SelectedItemActor)
        || SelectedTargetCoin != nullptr;
}

bool UUseableItemWSubsystem::TryCancelCurrentItemSelection()
{
    if(!HasPendingItemSelection())
    {
        return false;
    }

    CancelWantUseItem();
    return true;
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

    if(!TargetGrid || TargetGrid->GetIsOccupied() || TargetGrid->GetItemFlag() == 0)
    {
        PlayItemFailedFeedback();
        return;
    }

    if(SelectedTargetCoin && !ValidTargetGrids.Contains(TargetGrid->GetGridPoint()))
    {
        PlayItemFailedFeedback();
        return;
    }

    if(SelectedItemAction)
    {
        SelectedItemAction->SetTargetGrid(TargetGrid);
        SelectedItemAction->ExecuteAction();
        PlaySelectedItemSuccessVFX(TargetGrid, SelectedTargetCoin, nullptr);
        ConsumeSelectedItemActor();
    }
    CancelWantUseItem();
}

void UUseableItemWSubsystem::ExecuteItemForCoin(ACoinActor* TargetCoin)
{
    if(CurrentTargetMode == EUseableItemTargetMode::CoinThenGrid)
    {
        if(!TargetCoin || !SelectedItemAction || !SelectedItemActor) 
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

        ConsumeSelectedItemActorOnly();
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

    if(!TargetCoin || !SelectedItemAction)
    {
        PlayItemFailedFeedback();
        return;
    }

    if(TargetCoin && SelectedItemAction)
    {
        SelectedItemAction->SetInRangeCoins(TargetCoin);
        if(GridManager)
        {
            SelectedItemAction->SetTargetGrid(GridManager->GetGridActor(TargetCoin->GetDecidedGrid()));
        }
        SelectedItemAction->ExecuteAction();
        PlaySelectedItemSuccessVFX(nullptr, TargetCoin, nullptr);
        ConsumeSelectedItemActor();
    }
    CancelWantUseItem();
}
/*
bool UUseableItemWSubsystem::IsItemSetupInGrid() const
{
    //2가 설치
    if(SelectedItemAction->GetItemTypeID() == 2)
    {
        return true;
    }
    return false;
}
*/
void UUseableItemWSubsystem::VisibleItemInfoUI(AUseableItemActor* TargetItem)
{
    if(!TargetItem) return;

    if(ItemInfoWidgetInstance)
    {
        FItemData HoveredItemData;
        if(TryGetItemData(TargetItem, HoveredItemData))
        {
            ItemInfoWidgetInstance->UpdateItemInfo(HoveredItemData);
            ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void UUseableItemWSubsystem::HideItemInfoUi()
{
    if(CurrentTargetMode != EUseableItemTargetMode::None || IsValid(SelectedItemActor))
    {
        return;
    }

    if(ItemInfoWidgetInstance)
    {
        ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }

    if(CurrentTargetMode == EUseableItemTargetMode::None)
    {
        InitSelectedItem();
    }
}

void UUseableItemWSubsystem::SelectWantUseGridItem(AUseableItemActor* TargetItem)
{
    if(!bIsCoinSelectTurn || !TargetItem) return;
    if(CoinActionManager && CoinActionManager->IsActionSequenceActive()) return;

    if(CoinActionManager)
    {
        CoinActionManager->CancelSelectWeapon();
    }
    CancelWantUseItem();
    if(CoinActionManager)
    {
        CoinActionManager->SetTurn(false);
    }
    SetItemInfo(TargetItem);
    CurrentTargetMode = EUseableItemTargetMode::Grid;
    SelectedItemActor = TargetItem;
    bHasSelectedItemData = TryGetItemData(TargetItem, SelectedItemData);
    VisibleItemInfoUI(TargetItem);
    StartItemCursorPreview(TargetItem);

    if(CoinManager)
    {
        CoinManager->SetBattleCoinItemFlags(true);
    }

    if(GridManager)
    {
        GridManager->SetGridClickFlag(EGridClickFlag::ItemAction);
        GridManager->SetGridItemFlags(1);
    }
}

void UUseableItemWSubsystem::SelectWantUseCoinItem(AUseableItemActor* TargetItem)
{
    if(!bIsCoinSelectTurn || !TargetItem) return;
    if(CoinActionManager && CoinActionManager->IsActionSequenceActive()) return;

    if(CoinActionManager)
    {
        CoinActionManager->CancelSelectWeapon();
    }
    CancelWantUseItem();
    if(CoinActionManager)
    {
        CoinActionManager->SetTurn(false);
    }
    SetItemInfo(TargetItem);
    CurrentTargetMode = TargetItem->GetItemID() == EverywherePotionItemID
        ? EUseableItemTargetMode::CoinThenGrid
        : EUseableItemTargetMode::Coin;
    SelectedItemActor = TargetItem;
    bHasSelectedItemData = TryGetItemData(TargetItem, SelectedItemData);
    VisibleItemInfoUI(TargetItem);
    StartItemCursorPreview(TargetItem);

    if(CoinManager)
    {
        CoinManager->SetBattleCoinItemFlags(true);
    }

    if(GridManager)
    {
        GridManager->SetGridClickFlag(EGridClickFlag::None, false);
        GridManager->SetGridHoverFlags(2);
    }

}

void UUseableItemWSubsystem::HandleItemRightClicked(AUseableItemActor* TargetItem)
{
    if(!bIsCoinSelectTurn || !TargetItem) return;
    if(TargetItem != SelectedItemActor) return;

    PlayItemFailedFeedback();
}

void UUseableItemWSubsystem::HandleBattleCoinHoveredForItem(ACoinActor* HoveredCoin)
{
    if(!bIsCoinSelectTurn || !IsValid(HoveredCoin) || !IsValid(SelectedItemActor))
    {
        return;
    }

    if(CurrentTargetMode == EUseableItemTargetMode::None || SelectedItemActor->GetItemID() != PhaseChangePotionItemID)
    {
        return;
    }

    if(ItemInfoWidgetInstance)
    {
        ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }

    if(CoinActionManager)
    {
        CoinActionManager->PreviewCoinInfoForItemTarget(HoveredCoin, true);
    }
}

void UUseableItemWSubsystem::HandleBattleCoinUnhoveredForItem()
{
    if(CurrentTargetMode == EUseableItemTargetMode::None)
    {
        return;
    }

    if(CoinActionManager)
    {
        CoinActionManager->HideCoinInfoForItemTarget();
    }

    if(IsValid(SelectedItemActor) && SelectedItemActor->GetItemID() == PhaseChangePotionItemID)
    {
        VisibleItemInfoUI(SelectedItemActor);
    }
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
            Coin->OnHoverBattleCoin.AddUniqueDynamic(this, &UUseableItemWSubsystem::HandleBattleCoinHoveredForItem);
            Coin->OnUnhoverCoin.AddUniqueDynamic(this, &UUseableItemWSubsystem::HandleBattleCoinUnhoveredForItem);
        }
    }
}

void UUseableItemWSubsystem::SetTurn(const bool bIsTurn)
{
    bIsCoinSelectTurn = bIsTurn;

    if(!bIsCoinSelectTurn)
    {
        CancelWantUseItem();
        if(ItemInfoWidgetInstance)
        {
            ItemInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

int32 UUseableItemWSubsystem::CalculateItemPrice() const
{
    int32 ReturnItemPrice = 0;
    for(AUseableItemActor* Item : UnUsedItemArray)
    {
        if(IsValid(Item))
        {
            ReturnItemPrice += Item->GetItemPrice();
        }
    }

    return static_cast<int32>(ReturnItemPrice / 2);
}

int32 UUseableItemWSubsystem::CalculateItemCount() const
{
    int32 ItemCount = 0;
    for (AUseableItemActor* Item : UnUsedItemArray)
    {
        if (IsValid(Item))
        {
            ItemCount++;
        }
    }

    return ItemCount;
}
