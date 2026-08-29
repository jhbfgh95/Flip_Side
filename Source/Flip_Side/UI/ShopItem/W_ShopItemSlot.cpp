// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemSlot.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UW_ShopItemSlot::NativeConstruct()
{
    Super::NativeConstruct();

    ItemCountPlusButton->OnClicked.AddDynamic(this, &UW_ShopItemSlot::ClickItemCountPlusButton);
    ItemCountMinusButton->OnClicked.AddDynamic(this, &UW_ShopItemSlot::ClickItemCountMinusButton);
    ItemBuyButton->OnClicked.AddDynamic(this, &UW_ShopItemSlot::ClickItemBuyButton);
}


void UW_ShopItemSlot::InitItemWidget(FItemData SetItemData)
{
    WidgetItemData = SetItemData;
    ItemImage->SetBrushFromTexture(SetItemData.ItemIcon);
    ItemNameTextBlock->SetText(FText::FromString(SetItemData.ItemName));
    ItemCountTextBlock->SetText(FText::AsNumber(1));
    ItemPriceTextBlock->SetText(FText::AsNumber(SetItemData.Price));
    CurrentItemCount = 1;
}
	
void UW_ShopItemSlot::SetItemSlotImage(FItemData SetItemData)
{
    WidgetItemData = SetItemData;
    ItemImage->SetBrushFromTexture(SetItemData.ItemIcon);
    ItemNameTextBlock->SetText(FText::FromString(SetItemData.ItemName));
    ItemCountTextBlock->SetText(FText::AsNumber(1));
    ItemPriceTextBlock->SetText(FText::AsNumber(SetItemData.Price));
}

void UW_ShopItemSlot::AddBuyItemCount(int32 Count)
{
    CurrentItemCount++;
    ItemPriceTextBlock->SetText(FText::AsNumber(CurrentItemCount*WidgetItemData.Price));
    ItemCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}

void UW_ShopItemSlot::ClickItemBuyButton()
{

    ItemCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
    
    OnBuyItem.Broadcast(WidgetItemData.ItemID, CurrentItemCount);
}

void UW_ShopItemSlot::ClickItemCountPlusButton()
{
    OnAddBuyItemCount.Broadcast(this, WidgetItemData.ItemID, CurrentItemCount+1);
}
	
void UW_ShopItemSlot::ClickItemCountMinusButton()
{
    if(CurrentItemCount-1<1)
        return;

    CurrentItemCount--;

    ItemPriceTextBlock->SetText(FText::AsNumber(CurrentItemCount*WidgetItemData.Price));
    ItemCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}

void UW_ShopItemSlot::SetItemCount(int32 Count)
{
    
    CurrentItemCount = Count;
    ItemPriceTextBlock->SetText(FText::AsNumber(CurrentItemCount*WidgetItemData.Price));
    ItemCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}

void UW_ShopItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    OnHoveredSlot.Broadcast(WidgetItemData.ItemID);

}

void UW_ShopItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    OnUnhoveredSlot.Broadcast();
}
