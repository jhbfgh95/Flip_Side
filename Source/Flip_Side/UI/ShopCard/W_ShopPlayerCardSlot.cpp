// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UW_ShopPlayerCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (IsValid(DetailButton))
    {
        DetailButton->OnHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        DetailButton->OnUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
    }
    UpdateDetailButton(INDEX_NONE);
}

void UW_ShopPlayerCardSlot::UpdateDetailButton(int32 CardID)
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

void UW_ShopPlayerCardSlot::HandleDetailButtonHovered()
{
    if (bHasCardDetails && !bDetailButtonHovered)
    {
        bDetailButtonHovered = true;
        OnDetailButtonHovered.Broadcast();
    }
}

void UW_ShopPlayerCardSlot::HandleDetailButtonUnhovered()
{
    if (bDetailButtonHovered)
    {
        bDetailButtonHovered = false;
        OnDetailButtonUnhovered.Broadcast();
    }
}

void UW_ShopPlayerCardSlot::NativeDestruct()
{
    HandleDetailButtonUnhovered();
    if (IsValid(DetailButtonHighlightAnimation))
    {
        StopAnimation(DetailButtonHighlightAnimation);
    }
    Super::NativeDestruct();
}

void UW_ShopPlayerCardSlot::InitCardSlot(FCardData InitCard)
{
	UE_LOG(LogTemp, Warning, TEXT("[ShopPlayerCardSlot::InitCardSlot] CardName: %s"), *InitCard.CardName);
    WidgetCardData = InitCard;
    UpdateDetailButton(WidgetCardData.CardID);
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

void UW_ShopPlayerCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
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

FReply UW_ShopPlayerCardSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ClickCardButton();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
