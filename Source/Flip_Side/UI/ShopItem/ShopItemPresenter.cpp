// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/ShopItemPresenter.h"

#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
    
#include "UI/ShopItem/W_ShopItemWidget.h"
#include "UI/ShopItem/W_ShopItemSlotContainer.h"
#include "UI/ShopItem/W_ShopPlayerItemSlotContainer.h"
#include "UI/ShopItem/W_ShopItemSlot.h"
#include "UI/ShopItem/W_ShopPlayerItemSlot.h"
#include "UI/ShopItem/W_ShopItemDescription.h"
#include "UI/ShopItem/W_ShopItemPurchasePopup.h"
#include "UI/ShopItem/W_ShopItemSellPopup.h"
#include "UI/ShopItem/ShopItemUIActor.h"


void UShopItemPresenter::InitPresenter(UW_ShopItemWidget* InShopItemWidget, UShopItemWSubsystem* InItemSubsystem,
    UDataManagerSubsystem* InDataManager, AShopItemUIActor* InShopItemUIActor)
{
    ShopItemWidget = InShopItemWidget;
    ItemSubsystem = InItemSubsystem;
    DataManager = InDataManager;
    ShopItemUIActor = InShopItemUIActor;


    ItemSubsystem->OnItemBuy.AddDynamic(this, &UShopItemPresenter::SetPlayerItemSlot);

    SetShopItemPurchasePopup();
    SetShopItemSellPopup();
    SetShopSlotItemViews();
    SetPlayerSlotItemViews();
    
}

void UShopItemPresenter::BuyItem(int32 ItemID, int32 Count)
{
    const FItemData ItemData = GetItemData(ItemID);
    SetItemDescription(ItemData);
    ItemSubsystem->BuyItem(ItemData, Count);
}

void UShopItemPresenter::SellItem(int32 Index, int32 ItemID, int32 Count)
{
    
    const FItemData ItemData = GetItemData(ItemID);
    SetItemDescription(ItemData);
    const bool bSold = ItemSubsystem->SellItem(ItemData, Count);

    if (bSold && IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->PlayBuyAnimation();
    }
    
    int32 IndexItemCount = ItemSubsystem->GetSameItemCountByIndex(Index);

    if(0<IndexItemCount)
        ShopPlayerItemSlotViews[Index]->UpdateItemCount(IndexItemCount);
    else
        ShopPlayerItemSlotViews[Index]->DeleteItemWidget();
}

void UShopItemPresenter::SetPlayerItemSlot(int32 SetSlotIndex)
{
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->PlayBuyAnimation();
    }

    FSelectItem PlayerInvenItem = ItemSubsystem->GetPlayerItem(SetSlotIndex);
    if(!ShopPlayerItemSlotViews.IsValidIndex(SetSlotIndex))
        return;

    FItemData InvenItemData = GetItemData(PlayerInvenItem.ItemID);
    ShopPlayerItemSlotViews[SetSlotIndex]->SetItemWidget(InvenItemData, PlayerInvenItem);

}

void UShopItemPresenter::HoveredItemSlot(int32 ItemID)
{
    const FItemData ItemData = GetItemData(ItemID);
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->SetItemData(ItemData);
    }
    SetItemDescription(ItemData);
    //보이는 아이템에서 보이도록 설정
}
	
void UShopItemPresenter::UnhoveredItemSlot()
{
    if(IsPurchasePopUpOpen)
        return;
        
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->RemoveMaterial();
    }
    HideItemDescription();
}

void UShopItemPresenter::HoveredPlayerItemSlot(int32 ItemIndex)
{
    const FItemData ItemData = ItemSubsystem->GetPlayerItemData(ItemIndex);
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->SetItemData(ItemData);
    }
    SetItemDescription(ItemData);
}

void UShopItemPresenter::UnhoveredPlayerItemSlot()
{
    if(IsItemSellPopUpOpen)
        return;
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->RemoveMaterial();
    }
    HideItemDescription();
}
	

void UShopItemPresenter::SetShopSlotItemViews()
{
    ShopItemWidget->GetShopItemSlotContainer()->InitWidget(ItemSubsystem->GetShopItemList());
    ShopItemSlotViews = ShopItemWidget->GetShopItemSlotContainer()->GetShopItemSlots();

    for(int i =0; i< ShopItemSlotViews.Num();i++)
    {
        ShopItemSlotViews[i]->OnClickShopItemSlot.AddDynamic(this, &UShopItemPresenter::OpenPurchasePopup);
        ShopItemSlotViews[i]->OnHoveredSlot.AddDynamic(this, &UShopItemPresenter::HoveredItemSlot);
        ShopItemSlotViews[i]->OnUnhoveredSlot.AddDynamic(this, &UShopItemPresenter::UnhoveredItemSlot);
    }
}

void UShopItemPresenter::SetShopItemPurchasePopup()
{
    if (!IsValid(ShopItemWidget))
        return;

    if (UW_ShopItemPurchasePopup* Popup = ShopItemWidget->GetShopItemPurchasePopup())
    {
        Popup->OnPurchaseItemRequested.AddDynamic(this, &UShopItemPresenter::RequestPurchaseFromPopup);
        Popup->OnCancelled.AddDynamic(this, &UShopItemPresenter::ClosePurchasePopup);
    }
}

void UShopItemPresenter::SetShopItemSellPopup()
{
    if (!IsValid(ShopItemWidget))
        return;

    if (UW_ShopItemSellPopup* Popup = ShopItemWidget->GetShopItemSellPopup())
    {
        Popup->OnSellItemRequested.AddDynamic(this, &UShopItemPresenter::RequestSellFromPopup);
        Popup->OnCancelled.AddDynamic(this, &UShopItemPresenter::CloseSellPopup);
    }
}

void UShopItemPresenter::OpenPurchasePopup(UW_ShopItemSlot* ClickedSlot, int32 ItemID)
{
    if (!IsValid(ClickedSlot) || !IsValid(ShopItemWidget))
        return;
    
    FItemData SetItemData = GetItemData(ItemID);

    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->SetItemData(SetItemData);
    }
    SetItemDescription(SetItemData);

    if (UW_ShopItemPurchasePopup* Popup = ShopItemWidget->GetShopItemPurchasePopup())
    {
        IsPurchasePopUpOpen = true;
        Popup->Open(SetItemData);
    }
}

void UShopItemPresenter::RequestPurchaseFromPopup(int32 ItemID, int32 Count)
{
    const FItemData ItemData = GetItemData(ItemID);
    if (!ItemSubsystem->CanBuyItem(ItemData.Price, Count))
        return;

    BuyItem(ItemID, Count);
    ClosePurchasePopup();
}

void UShopItemPresenter::ClosePurchasePopup()
{
    if (!IsValid(ShopItemWidget))
        return;

    if (UW_ShopItemPurchasePopup* Popup = ShopItemWidget->GetShopItemPurchasePopup())
    {
        Popup->Close();
        IsPurchasePopUpOpen = false;
    }

    if (IsPurchasePopUpOpen)
        return;

    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->RemoveMaterial();
    }
    HideItemDescription();

}

void UShopItemPresenter::OpenSellPopup(UW_ShopPlayerItemSlot* ClickedSlot, int32 InventoryIndex)
{
    if (!IsValid(ClickedSlot) || !IsValid(ShopItemWidget) || !ItemSubsystem->GetPlayerItemArray().IsValidIndex(InventoryIndex))
        return;

    const FSelectItem PlayerItem = ItemSubsystem->GetPlayerItem(InventoryIndex);
    if (PlayerItem.ItemID == -1 || PlayerItem.SameItemNum <= 0)
        return;

    const FItemData ItemData = GetItemData(PlayerItem.ItemID);
    if (ItemData.ItemID == -1)
        return;

    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->SetItemData(ItemData);
    }
    SetItemDescription(ItemData);

    if (UW_ShopItemSellPopup* Popup = ShopItemWidget->GetShopItemSellPopup())
    {
        IsItemSellPopUpOpen = true;
        Popup->Open(InventoryIndex, ItemData, PlayerItem.SameItemNum);
    }
}

void UShopItemPresenter::RequestSellFromPopup(int32 InventoryIndex, int32 ItemID, int32 Count)
{
    if (!ItemSubsystem->GetPlayerItemArray().IsValidIndex(InventoryIndex))
        return;

    const FSelectItem PlayerItem = ItemSubsystem->GetPlayerItem(InventoryIndex);
    if (PlayerItem.ItemID != ItemID || Count <= 0 || Count > PlayerItem.SameItemNum)
        return;

    SellItem(InventoryIndex, ItemID, Count);
    CloseSellPopup();
}

void UShopItemPresenter::CloseSellPopup()
{
    if (!IsValid(ShopItemWidget))
        return;

    if (UW_ShopItemSellPopup* Popup = ShopItemWidget->GetShopItemSellPopup())
    {
        Popup->Close();
        IsItemSellPopUpOpen = false;
    }

    if (IsItemSellPopUpOpen)
        return;

    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->RemoveMaterial();
    }
    HideItemDescription();
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
        ShopPlayerItemSlotViews[i]->OnRequestSellPopup.AddDynamic(this, &UShopItemPresenter::OpenSellPopup);
        ShopPlayerItemSlotViews[i]->OnHoveredSlot.AddDynamic(this, &UShopItemPresenter::HoveredPlayerItemSlot);
        ShopPlayerItemSlotViews[i]->OnUnhoveredSlot.AddDynamic(this, &UShopItemPresenter::UnhoveredPlayerItemSlot);
    }
}

void UShopItemPresenter::SetItemDescription(const FItemData& ItemData)
{
    if (!IsValid(ShopItemWidget) || ItemData.ItemID == -1)
    {
        HideItemDescription();
        return;
    }

    if (UW_ShopItemDescription* DescriptionWidget = ShopItemWidget->GetShopItemDescription())
    {
        DescriptionWidget->SetItemInfo(ItemData.ItemName, ItemData.Item_DES);
    }
}

void UShopItemPresenter::HideItemDescription()
{
    if (IsValid(ShopItemWidget))
    {
        if (UW_ShopItemDescription* DescriptionWidget = ShopItemWidget->GetShopItemDescription())
        {
            DescriptionWidget->ResetItemWidget();
        }
    }
}
	
FItemData UShopItemPresenter::GetItemData(int32 ID)
{
    FItemData ReturnItemData;
    ReturnItemData.ItemID = -1;
    DataManager->TryGetItem(ID,ReturnItemData);
    return ReturnItemData;
}
