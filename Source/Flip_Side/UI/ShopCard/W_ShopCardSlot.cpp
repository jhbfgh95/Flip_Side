// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopCard/W_ShopCardSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_ShopCardSlot::NativeConstruct()
{
    Super::NativeConstruct();

    CardImageMI = CardImage->GetDynamicMaterial();
}

void UW_ShopCardSlot::InitCardSlot(FCardData InitCard)
{
    WidgetCardData = InitCard;

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
    this->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopCardSlot::ClickBuyCardButton()
{
    OnBuyCard.Broadcast(this, WidgetCardData.CardID);
}

void UW_ShopCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }

    OnUnhoveredShopCardSlot.Broadcast();
}

FReply UW_ShopCardSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ClickBuyCardButton();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
