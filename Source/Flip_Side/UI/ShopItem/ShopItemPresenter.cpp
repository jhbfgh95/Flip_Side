// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/ShopItemPresenter.h"

#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
    
#include "UI/ShopItem/W_ShopItemWidget.h"
#include "UI/ShopItem/W_ShopItemSlotContainer.h"
#include "UI/ShopItem/W_ShopPlayerItemSlotContainer.h"
#include "UI/ShopItem/W_ShopItemSlot.h"
#include "UI/ShopItem/W_ShopPlayerItemSlot.h"
#include "UI/ShopItem/W_ShopSelectedItem.h"
#include "UI/ShopItem/W_ShopItemDescription.h"
#include "UI/ShopItem/ShopItemUIActor.h"


void UShopItemPresenter::InitPresenter(UW_ShopItemWidget* InShopItemWidget, UShopItemWSubsystem* InItemSubsystem,
    UDataManagerSubsystem* InDataManager, AShopItemUIActor* InShopItemUIActor)
{
    ShopItemWidget = InShopItemWidget;
    ItemSubsystem = InItemSubsystem;
    DataManager = InDataManager;
    ShopItemUIActor = InShopItemUIActor;


    ItemSubsystem->OnItemBuy.AddDynamic(this, &UShopItemPresenter::SetPlayerItemSlot);

    SetShopSlotItemViews();
    SetPlayerSlotItemViews();
    
}

void UShopItemPresenter::BuyItem(int32 ItemID, int32 Count)
{
    const FItemData ItemData = GetItemData(ItemID);
    SetSelectedItemImage(ItemData);
    SetItemDescription(ItemData);
    ItemSubsystem->BuyItem(ItemData, Count);
}

void UShopItemPresenter::SellItem(int32 Index, int32 ItemID, int32 Count)
{
    
    const FItemData ItemData = GetItemData(ItemID);
    SetSelectedItemImage(ItemData);
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
    SetSelectedItemImage(ItemData);
    SetItemDescription(ItemData);
    //보이는 아이템에서 보이도록 설정
}
	
void UShopItemPresenter::UnhoveredItemSlot()
{
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->RemoveMaterial();
    }
    if (IsValid(ShopItemWidget) && IsValid(ShopItemWidget->GetShopSelectedItem()))
    {
        ShopItemWidget->GetShopSelectedItem()->SetImage(nullptr);
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
    SetSelectedItemImage(ItemData);
    SetItemDescription(ItemData);
}

void UShopItemPresenter::UnhoveredPlayerItemSlot()
{
    if (IsValid(ShopItemUIActor))
    {
        ShopItemUIActor->RemoveMaterial();
    }
    if (IsValid(ShopItemWidget) && IsValid(ShopItemWidget->GetShopSelectedItem()))
    {
        ShopItemWidget->GetShopSelectedItem()->SetImage(nullptr);
    }
    HideItemDescription();
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
        ShopPlayerItemSlotViews[i]->OnHoveredSlot.AddDynamic(this, &UShopItemPresenter::HoveredPlayerItemSlot);
        ShopPlayerItemSlotViews[i]->OnUnhoveredSlot.AddDynamic(this, &UShopItemPresenter::UnhoveredPlayerItemSlot);
    }
}

void UShopItemPresenter::SetSelectedItemImage(const FItemData& ItemData)
{
    if (IsValid(ShopItemWidget) && IsValid(ShopItemWidget->GetShopSelectedItem()))
    {
        ShopItemWidget->GetShopSelectedItem()->SetImage(ItemData.ItemIcon);
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
