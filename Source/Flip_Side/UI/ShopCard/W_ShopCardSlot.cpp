// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardSlot.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_ShopCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    BuyCardButton->OnClicked.AddDynamic(this, &UW_ShopCardSlot::BuyCard);
}


void UW_ShopCardSlot::InitCardSlot(FCardData InitCard, UShopCardWSubsystem* InitCardSubsystem)
{
    WidgetCardData = InitCard;
    CardSubsystem = InitCardSubsystem;

    CardImage->SetBrushFromTexture(WidgetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));
    CardPriceTextBlock->SetText(FText::AsNumber(WidgetCardData.Price));
}

void UW_ShopCardSlot::BuyCard()
{
    if(!CardSubsystem)
        return;
    CardSubsystem->BuyCard(WidgetCardData);
}

void UW_ShopCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    CardSubsystem->HoverCardSlot(WidgetCardData);
}

void UW_ShopCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    CardSubsystem->UnhoverCardSlot();
}
