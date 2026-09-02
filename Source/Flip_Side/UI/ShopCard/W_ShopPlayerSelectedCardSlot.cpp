// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerSelectedCardSlot.h"
#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UW_ShopPlayerSelectedCardSlot::InitCardSlot(int32 InIndex)
{
    SlotIndex = InIndex;
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

void UW_ShopPlayerSelectedCardSlot::ClickUnSelectCard()
{
    OnUnselectedPlayerCard.Broadcast(SlotIndex);
}
	
void UW_ShopPlayerSelectedCardSlot::SetCardSlot(FCardData SetCardData, UW_ShopPlayerCardSlot* ConnectedPlayerCardSlot)
{
    WidgetCardData = SetCardData;
    CardImage->SetBrushFromTexture(SetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(SetCardData.CardName));
    PlayerCardSlot = ConnectedPlayerCardSlot;
    IsSelected = true;

    if (SelectBorder)
    {
        SelectBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UW_ShopPlayerSelectedCardSlot::DeactiveSlot()
{
    PlayerCardSlot->SetSlotIsSelected(false);
    IsSelected = false;

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    if (SelectBorder)
    {
        SelectBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    this->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopPlayerSelectedCardSlot::ClearSlot()
{
    WidgetCardData = FCardData();
    WidgetCardData.CardID = -1;
    PlayerCardSlot = nullptr;
    IsSelected = false;

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    if (SelectBorder)
    {
        SelectBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    SetVisibility(ESlateVisibility::Collapsed);
}


void UW_ShopPlayerSelectedCardSlot::UpdateSlot()
{
    CardImage->SetBrushFromTexture(WidgetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));
}

FCardData UW_ShopPlayerSelectedCardSlot::GetCardData()
{   
    return WidgetCardData;
}
UW_ShopPlayerCardSlot* UW_ShopPlayerSelectedCardSlot::GetConnnectedSlot()
{
    return PlayerCardSlot;
}

void UW_ShopPlayerSelectedCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    
    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopPlayerSelectedCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    
    OnUnhoveredShopCardSlot.Broadcast();
}

FReply UW_ShopPlayerSelectedCardSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ClickUnSelectCard();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
