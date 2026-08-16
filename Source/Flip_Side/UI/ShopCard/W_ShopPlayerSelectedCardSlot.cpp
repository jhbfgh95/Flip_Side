// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerSelectedCardSlot.h"
#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"


	
void UW_ShopPlayerSelectedCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    UnSelectCardButton->OnClicked.AddDynamic(this, &UW_ShopPlayerSelectedCardSlot::ClickUnSelectCard);
}

void UW_ShopPlayerSelectedCardSlot::InitCardSlot(int32 InIndex)
{
    SlotIndex = InIndex;
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
}

void UW_ShopPlayerSelectedCardSlot::DeactiveSlot()
{
    PlayerCardSlot->SetSlotIsSelected(false);
    this->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopPlayerSelectedCardSlot::ClearSlot()
{
    WidgetCardData = FCardData();
    WidgetCardData.CardID = -1;
    PlayerCardSlot = nullptr;
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
    
    OnHoveredShopCardSlot.Broadcast(WidgetCardData.CardID);
}

void UW_ShopPlayerSelectedCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    
    OnUnhoveredShopCardSlot.Broadcast();
}
