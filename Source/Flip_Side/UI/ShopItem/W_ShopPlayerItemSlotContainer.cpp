// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopPlayerItemSlotContainer.h"
#include "UI/ShopItem/W_ShopPlayerItemSlot.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopItem/W_ShopItemSlot.h"

void UW_ShopPlayerItemSlotContainer::InitWidget(TArray<FItemData> InItemDataArray, TArray<FSelectItem> InSelectItemData)
{
    FItemData DefaultsItemData;
    DefaultsItemData.ItemID = -1;
    FSelectItem DefaultsSelectItem;
    DefaultsSelectItem.ItemID = -1;
    for(int i =0; i<3;i++)
    {
        UW_ShopPlayerItemSlot* ItemSlotWidget =Cast<UW_ShopPlayerItemSlot>(CreateWidget<UUserWidget>(GetWorld(), PlayerItemSlot));
        if (ItemSlotWidget)
        {
            PlayerItemSlots.Add(ItemSlotWidget);
            UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(ItemSlotWidget);
            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            if(InItemDataArray.IsValidIndex(i) && InSelectItemData.IsValidIndex(i))
                ItemSlotWidget->InitItemWidget(i, InItemDataArray[i], InSelectItemData[i]);
            else
                ItemSlotWidget->InitItemWidget(i, DefaultsItemData, DefaultsSelectItem);

        }
    }
}
	
TArray<TObjectPtr<UW_ShopPlayerItemSlot>> UW_ShopPlayerItemSlotContainer::GetPlayerItemSlots()
{
    return PlayerItemSlots;
}