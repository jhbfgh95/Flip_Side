// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/ShopItemPresenter.h"

#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
    
#include "UI/ShopItem/W_ShopItemWidget.h"
#include "UI/ShopItem/W_ShopItemSlotContainer.h"
#include "UI/ShopItem/W_ShopPlayerItemSlotContainer.h"
#include "UI/ShopItem/W_ShopItemSlot.h"
#include "UI/ShopItem/W_ShopPlayerItemSlot.h"


void UShopItemPresenter::InitPresenter(UW_ShopItemWidget* InShopItemWidget, UShopItemWSubsystem* InItemSubsystem, UDataManagerSubsystem* InDataManager)
{
    ShopItemWidget = InShopItemWidget;
    ItemSubsystem = InItemSubsystem;
    DataManager = InDataManager;


    ItemSubsystem->OnItemBuy.AddDynamic(this, &UShopItemPresenter::SetPlayerItemSlot);

    SetShopSlotItemViews();
    SetPlayerSlotItemViews();
    
}

void UShopItemPresenter::BuyItem(int32 ItemID, int32 Count)
{
    ItemSubsystem->BuyItem(GetItemData(ItemID), Count);
}

void UShopItemPresenter::SellItem(int32 Index, int32 ItemID, int32 Count)
{
    
    ItemSubsystem->SellItem(GetItemData(ItemID), Count);
    
    int32 IndexItemCount = ItemSubsystem->GetSameItemCountByIndex(Index);

    if(0<IndexItemCount)
        ShopPlayerItemSlotViews[Index]->UpdateItemCount(IndexItemCount);
    else
        ShopPlayerItemSlotViews[Index]->DeleteItemWidget();
}

void UShopItemPresenter::SetPlayerItemSlot(int32 SetSlotIndex)
{
    FSelectItem PlayerInvenItem = ItemSubsystem->GetPlayerItem(SetSlotIndex);
    if(!ShopPlayerItemSlotViews.IsValidIndex(SetSlotIndex))
        return;

    FItemData InvenItemData = GetItemData(PlayerInvenItem.ItemID);
    ShopPlayerItemSlotViews[SetSlotIndex]->SetItemWidget(InvenItemData, PlayerInvenItem);

}

void UShopItemPresenter::HoveredItemSlot(int32 ItemID)
{
    ItemSubsystem->HoverItem(GetItemData(ItemID));
    //보이는 아이템에서 보이도록 설정
}
	
void UShopItemPresenter::UnhoveredItemSlot()
{
    ItemSubsystem->UnHoverItem();
    //안보이도록 설정
}
	

void UShopItemPresenter::SetShopSlotItemViews()
{
    ShopItemWidget->GetShopItemSlotContainer()->InitWidget(ItemSubsystem->GetShopItemList());
    ShopItemSlotViews = ShopItemWidget->GetShopItemSlotContainer()->GetShopItemSlots();

    for(int i =0; i< ShopItemSlotViews.Num();i++)
    {
        ShopItemSlotViews[i]->OnBuyItem.AddDynamic(this, &UShopItemPresenter::BuyItem);
        ShopItemSlotViews[i]->OnHoveredSlot.AddDynamic(this, &UShopItemPresenter::HoveredItemSlot);
        ShopItemSlotViews[i]->OnUnhoveredSlot.AddDynamic(this, &UShopItemPresenter::UnhoveredItemSlot);
        ShopItemSlotViews[i]->OnAddBuyItemCount.AddDynamic(this, &UShopItemPresenter::CheckCanItemBuy);
    }
}


void UShopItemPresenter::SetPlayerSlotItemViews()
{
    TArray<FSelectItem> PlayerSelectItemArray = ItemSubsystem->GetPlayerItemArray();
    TArray<FItemData> PlayerInvenItemData;

    for(int i =0; i<PlayerSelectItemArray.Num();i++)
    {
        PlayerInvenItemData.Add(GetItemData(PlayerSelectItemArray[i].ItemID));
    }

    ShopItemWidget->GetShopPlayerItemSlotContainer()->InitWidget(PlayerInvenItemData, PlayerSelectItemArray);
    ShopPlayerItemSlotViews = ShopItemWidget->GetShopPlayerItemSlotContainer()->GetPlayerItemSlots();

    for(int i =0; i < ShopPlayerItemSlotViews.Num(); i++)
    {
        ShopPlayerItemSlotViews[i]->OnSellItem.AddDynamic(this, &UShopItemPresenter::SellItem);
    }
}
	
void UShopItemPresenter::CheckCanItemBuy(UW_ShopItemSlot* BuyItemSlot, int32 BuyItemID, int32 Count)
{
    if(ItemSubsystem->CanBuyItem(GetItemData(BuyItemID).Price, Count))
    {
        BuyItemSlot->AddBuyItemCount(Count);
    }
}

FItemData UShopItemPresenter::GetItemData(int32 ID)
{
    FItemData ReturnItemData;
    ReturnItemData.ItemID = -1;
    DataManager->TryGetItem(ID,ReturnItemData);
    return ReturnItemData;
}