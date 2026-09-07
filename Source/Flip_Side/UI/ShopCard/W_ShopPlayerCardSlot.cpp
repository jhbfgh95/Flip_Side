// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UW_ShopPlayerCardSlot::InitCardSlot(FCardData InitCard)
{
    WidgetCardData = InitCard;
    CardImage->SetBrushFromTexture(WidgetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));

    IsSelected = false;
    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    if (SelectBorder)
    {
        SelectBorder->SetVisibility(ESlateVisibility::Hidden);
    }
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

    if (SelectBorder)
    {
        if(IsSelected)
            SelectBorder->SetVisibility( ESlateVisibility::HitTestInvisible);
        else
            SelectBorder->SetVisibility( ESlateVisibility::Hidden);
    }
}

int32 UW_ShopPlayerCardSlot::GetSlotCardID()
{
    return WidgetCardData.CardID;
}

void UW_ShopPlayerCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopPlayerCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    
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
