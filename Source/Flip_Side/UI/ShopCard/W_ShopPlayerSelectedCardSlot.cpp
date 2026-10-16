// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerSelectedCardSlot.h"
#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UW_ShopPlayerSelectedCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (IsValid(DetailButton))
    {
        DetailButton->OnHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        DetailButton->OnUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
    }
    UpdateDetailButton(INDEX_NONE);
}

void UW_ShopPlayerSelectedCardSlot::UpdateDetailButton(int32 CardID)
{
    bHasCardDetails = CardID == 4;
    if (!bHasCardDetails)
    {
        HandleDetailButtonUnhovered();
    }
    if (IsValid(DetailButtonHighlightAnimation))
    {
        StopAnimation(DetailButtonHighlightAnimation);
    }
    if (IsValid(DetailButton))
    {
        DetailButton->SetVisibility(bHasCardDetails ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
    if (IsValid(DetailBorder))
    {
        DetailBorder->SetVisibility(bHasCardDetails
            ? (IsHovered() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden)
            : ESlateVisibility::Collapsed);
    }
    if (bHasCardDetails && IsHovered() && IsValid(DetailButtonHighlightAnimation))
    {
        PlayAnimation(DetailButtonHighlightAnimation, 0.f, 0);
    }
}

void UW_ShopPlayerSelectedCardSlot::HandleDetailButtonHovered()
{
    if (bHasCardDetails && !bDetailButtonHovered)
    {
        bDetailButtonHovered = true;
        OnDetailButtonHovered.Broadcast();
    }
}

void UW_ShopPlayerSelectedCardSlot::HandleDetailButtonUnhovered()
{
    if (bDetailButtonHovered)
    {
        bDetailButtonHovered = false;
        OnDetailButtonUnhovered.Broadcast();
    }
}

void UW_ShopPlayerSelectedCardSlot::NativeDestruct()
{
    HandleDetailButtonUnhovered();
    if (IsValid(DetailButtonHighlightAnimation))
    {
        StopAnimation(DetailButtonHighlightAnimation);
    }
    Super::NativeDestruct();
}

void UW_ShopPlayerSelectedCardSlot::InitCardSlot(int32 InIndex)
{
    SlotIndex = InIndex;
    IsSelected = false;
    UpdateDetailButton(INDEX_NONE);

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
    UpdateDetailButton(WidgetCardData.CardID);
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
    UpdateDetailButton(INDEX_NONE);
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
    UpdateDetailButton(INDEX_NONE);
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
    if (IsValid(DetailBorder))
    {
        DetailBorder->SetVisibility(bHasCardDetails ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    }
    if (bHasCardDetails && IsValid(DetailButtonHighlightAnimation))
    {
        PlayAnimation(DetailButtonHighlightAnimation, 0.f, 0);
    }

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    
    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopPlayerSelectedCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    if (IsValid(DetailButtonHighlightAnimation))
    {
        StopAnimation(DetailButtonHighlightAnimation);
    }
    HandleDetailButtonUnhovered();

    if (IsValid(DetailBorder))
    {
        DetailBorder->SetVisibility(bHasCardDetails ? ESlateVisibility::Hidden : ESlateVisibility::Collapsed);
    }

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
