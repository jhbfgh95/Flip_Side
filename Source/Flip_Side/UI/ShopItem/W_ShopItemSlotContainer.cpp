// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopItem/W_ShopItemSlotContainer.h"
#include "Components/VerticalBox.h"
#include "UI/ShopItem/W_ShopItemSlot.h"



void UW_ShopItemSlotContainer::InitWidget(TArray<FItemData> InItemDataArray)
{
    int32 ShopItemCount = InItemDataArray.Num();
    

    for(int i =0; i<ShopItemCount;i++)
    {
        UW_ShopItemSlot* ItemSlotWidget =Cast<UW_ShopItemSlot>(CreateWidget<UUserWidget>(GetWorld(), SellItemSlot));
        if (ItemSlotWidget)
        {
            ShopItemSlots.Add(ItemSlotWidget);
            ShopItemSlots[i]->InitItemWidget(InItemDataArray[i]);
            SlotBox->AddChildToVerticalBox(ItemSlotWidget);
        }
    }
}
	
TArray<TObjectPtr<UW_ShopItemSlot>> UW_ShopItemSlotContainer::GetShopItemSlots()
{
    return ShopItemSlots;
}