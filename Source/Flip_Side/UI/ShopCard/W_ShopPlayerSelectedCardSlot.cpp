// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerSelectedCardSlot.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"


	
void UW_ShopPlayerSelectedCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    UnSelectCardButton->OnClicked.AddDynamic(this, &UW_ShopPlayerSelectedCardSlot::ClickUnSelectCard);
}

void UW_ShopPlayerSelectedCardSlot::InitCardSlot(UShopCardWSubsystem* InitCardSubsystem)
{
    CardSubsystem = InitCardSubsystem;
}   


void UW_ShopPlayerSelectedCardSlot::ClickUnSelectCard()
{
    CardSubsystem->UnSelectPlayerCard(WidgetCardData);
    PlayerCardSlot->SetSlotIsSelected(false);
}
	
void UW_ShopPlayerSelectedCardSlot::SetCardSlot(FCardData SetCardData, UW_ShopPlayerCardSlot* ConnectedPlayerCardSlot)
{
    WidgetCardData = SetCardData;
    CardImage->SetBrushFromTexture(SetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(SetCardData.CardName));
    PlayerCardSlot = ConnectedPlayerCardSlot;
}


void UW_ShopPlayerSelectedCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    if(!CardSubsystem)
        return;
    CardSubsystem->HoverCardSlot(WidgetCardData);
}

void UW_ShopPlayerSelectedCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    if(!CardSubsystem)
        return;
    CardSubsystem->UnhoverCardSlot();
}