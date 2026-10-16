// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopCard/W_ShopCardSlot.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_ShopCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (IsValid(DetailButton))
    {
        DetailButton->OnHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        DetailButton->OnUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
    }
    UpdateDetailButton(INDEX_NONE);
}

void UW_ShopCardSlot::UpdateDetailButton(int32 CardID)
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

void UW_ShopCardSlot::HandleDetailButtonHovered()
{
    if (bHasCardDetails && !bDetailButtonHovered)
    {
        bDetailButtonHovered = true;
        OnDetailButtonHovered.Broadcast();
    }
}

void UW_ShopCardSlot::HandleDetailButtonUnhovered()
{
    if (bDetailButtonHovered)
    {
        bDetailButtonHovered = false;
        OnDetailButtonUnhovered.Broadcast();
    }
}

void UW_ShopCardSlot::NativeDestruct()
{
    HandleDetailButtonUnhovered();
    if (IsValid(DetailButtonHighlightAnimation))
    {
        StopAnimation(DetailButtonHighlightAnimation);
    }
    Super::NativeDestruct();
}

void UW_ShopCardSlot::NativeConstruct()
{
    Super::NativeConstruct();

    CardImageMI = CardImage->GetDynamicMaterial();
}

void UW_ShopCardSlot::InitCardSlot(FCardData InitCard)
{
    WidgetCardData = InitCard;
    UpdateDetailButton(WidgetCardData.CardID);

    if (!CardImageMI)
    {
        CardImageMI = CardImage->GetDynamicMaterial();
    }

    if (CardImageMI)
    {
        CardImageMI->SetTextureParameterValue(FName("Weapon_Icon"), WidgetCardData.Icon);
        CardImageMI->SetVectorParameterValue(FName("Weapon_Color"), CardIconColor);
    }
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));
    CardPriceTextBlock->SetText(FText::AsNumber(WidgetCardData.Price));
    
    HoverBorder->SetVisibility(ESlateVisibility::Hidden);

}

void UW_ShopCardSlot::SetCardSlotHidden()
{
    UpdateDetailButton(INDEX_NONE);
    this->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopCardSlot::SetCardSlotInputEnabled(bool bInEnabled)
{
    bCardSlotInputEnabled = bInEnabled;
    SetIsEnabled(bInEnabled);
}

void UW_ShopCardSlot::ClickBuyCardButton()
{
    OnBuyCard.Broadcast(this, WidgetCardData.CardID);
}

void UW_ShopCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
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

void UW_ShopCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
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

FReply UW_ShopCardSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bCardSlotInputEnabled)
    {
        return FReply::Unhandled();
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ClickBuyCardButton();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
