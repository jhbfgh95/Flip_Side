// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopCard/W_ShopCardSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_ShopCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    BuyCardButton->OnClicked.AddDynamic(this, &UW_ShopCardSlot::ClickBuyCardButton);
}

void UW_ShopCardSlot::InitCardSlot(FCardData InitCard)
{
    WidgetCardData = InitCard;

    CardImage->SetBrushFromTexture(WidgetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));
    CardPriceTextBlock->SetText(FText::AsNumber(WidgetCardData.Price));
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

    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    OnUnhoveredShopCardSlot.Broadcast();
}
