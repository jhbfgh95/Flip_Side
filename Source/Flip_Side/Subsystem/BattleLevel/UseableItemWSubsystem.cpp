// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystems/Subsystem.h"
#include "FlipSideDevloperSettings.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DataManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "UseableItemActor.h"
#include "FlipSide_Enum.h"
#include "SlotActor.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "ItemDataTypes.h"
#include "GridTypes.h"
#include "Item_Action.h"
#include "W_ItemInfo.h"

void UUseableItemWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitUseitemSlot();

    SelectedItemAction = NewObject<UItem_Action>(this);

    CoinManager = Collection.InitializeDependency<UCoinManagementWSubsystem>();
    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();

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
                                NewItem->SetItemValues(ItemData.ItemID, ItemData.ItemType,ItemData.ItemIcon, ItemData.TypeColor);
                                NewItem->FinishSpawning(SpawnTransform);
                                BindItemDelegates(NewItem);
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
    if(IsValid(SelectedItemActor))
    {
        SelectedItemActor->Destroy();
    }
    SelectedItemActor = nullptr;
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
    InitSelectedItem();
}


void UUseableItemWSubsystem::ExecuteItemForGrid(AGridActor* TargetGrid)
{
    if(CurrentTargetMode != EUseableItemTargetMode::Grid)
    {
        CancelWantUseItem();
        return;
    }

    if(!TargetGrid || TargetGrid->GetIsOccupied() || TargetGrid->GetItemFlag() == 0)
    {
        CancelWantUseItem();
        return;
    }

    if(SelectedItemAction)
    {
        SelectedItemAction->SetTargetGrid(TargetGrid);
        SelectedItemAction->ExecuteAction();
        ConsumeSelectedItemActor();
    }
    CancelWantUseItem();
}

void UUseableItemWSubsystem::ExecuteItemForCoin(ACoinActor* TargetCoin)
{
    if(CurrentTargetMode != EUseableItemTargetMode::Coin)
    {
        CancelWantUseItem();
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

    CancelWantUseItem();
    SetItemInfo(TargetItem);
    CurrentTargetMode = EUseableItemTargetMode::Grid;
    SelectedItemActor = TargetItem;

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

    CancelWantUseItem();
    SetItemInfo(TargetItem);
    CurrentTargetMode = EUseableItemTargetMode::Coin;
    SelectedItemActor = TargetItem;

    if(CoinManager)
    {
        CoinManager->SetBattleCoinItemFlags(true);
    }

    if(GridManager)
    {
        GridManager->SetGridClickFlag(EGridClickFlag::ItemAction, false);
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
