// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemSlot.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Input/Reply.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_ShopItemSlot::NativeConstruct()
{
    Super::NativeConstruct();

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }

    
    ItemIconMI = ItemImage->GetDynamicMaterial();
}


void UW_ShopItemSlot::InitItemWidget(FItemData SetItemData)
{
    WidgetItemData = SetItemData;
    if(!IsValid(ItemIconMI))
    {
        ItemIconMI = ItemImage->GetDynamicMaterial();
    }
    ItemIconMI->SetTextureParameterValue(FName("Weapon_Icon"), SetItemData.ItemIcon);
    ItemIconMI->SetVectorParameterValue(FName("Weapon_Color"), WidgetItemData.TypeColor);

    ItemNameTextBlock->SetText(FText::FromString(SetItemData.ItemName));
    ItemPriceTextBlock->SetText(FText::AsNumber(SetItemData.Price));
}
	
void UW_ShopItemSlot::SetItemSlotImage(FItemData SetItemData)
{
    WidgetItemData = SetItemData;
    
    ItemIconMI->SetTextureParameterValue(FName("Weapon_Icon"), SetItemData.ItemIcon);
    ItemIconMI->SetVectorParameterValue(FName("Weapon_Color"), SetItemData.TypeColor);

    ItemNameTextBlock->SetText(FText::FromString(SetItemData.ItemName));
    ItemPriceTextBlock->SetText(FText::AsNumber(SetItemData.Price));
}

FReply UW_ShopItemSlot::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        OnClickShopItemSlot.Broadcast(this, WidgetItemData.ItemID);
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}


void UW_ShopItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    OnHoveredSlot.Broadcast(WidgetItemData.ItemID);
}

void UW_ShopItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }

    OnUnhoveredSlot.Broadcast();
}
