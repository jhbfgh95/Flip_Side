// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UW_ShopPlayerCardSlot::InitCardSlot(FCardData InitCard)
{
    WidgetCardData = InitCard;
    CardImage->SetBrushFromTexture(WidgetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));
}

void UW_ShopPlayerCardSlot::ClickCardButton()
{
    if(!IsSelected)
    {
        OnSelectShopPlayerCard.Broadcast(this, WidgetCardData.CardID);
    }
    else
    {
        OnUnselectShopPlayerCard.Broadcast(WidgetCardData.CardID);
    }

}
	
void UW_ShopPlayerCardSlot::SetSlotIsSelected(bool SetIsSelected)
{
    IsSelected = SetIsSelected;
}

int32 UW_ShopPlayerCardSlot::GetSlotCardID()
{
    return WidgetCardData.CardID;
}

void UW_ShopPlayerCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopPlayerCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    
    OnUnhoveredShopCardSlot.Broadcast();
}

FReply UW_ShopPlayerCardSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ClickCardButton();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
