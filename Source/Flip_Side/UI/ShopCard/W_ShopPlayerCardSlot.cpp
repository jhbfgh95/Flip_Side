// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UW_ShopPlayerCardSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    SelectCardButton->OnClicked.AddDynamic(this, &UW_ShopPlayerCardSlot::SelectCard);
}


void UW_ShopPlayerCardSlot::InitCardSlot(FCardData InitCard, UShopCardWSubsystem* InitCardSubsystem)
{
    CardSubsystem = InitCardSubsystem;
    SetCardSlot(InitCard);
}

void UW_ShopPlayerCardSlot::SelectCard()
{
    if(!CardSubsystem)
        return;
    
    if(!IsSelected)
    {
        if(CardSubsystem->SelectPlayerCard(WidgetCardData))
        {
            IsSelected = true;
        }
    }
    else
    {
        if(CardSubsystem->UnSelectPlayerCard(WidgetCardData))
        {
            IsSelected = false;
        }
    }
}
	
void UW_ShopPlayerCardSlot::SetCardSlot(FCardData SetCardData)
{
    WidgetCardData = SetCardData;
    CardImage->SetBrushFromTexture(WidgetCardData.Icon);
    CardNameTextBlock->SetText(FText::FromString(WidgetCardData.CardName));
}

void UW_ShopPlayerCardSlot::SetSlotIsSelected(bool SetIsSelected)
{
    IsSelected = SetIsSelected;
}

int32 UW_ShopPlayerCardSlot::GetSlotCardID()
{
    return WidgetCardData.CardID;
}

void UW_ShopPlayerCardSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    if(!CardSubsystem)
        return;
    CardSubsystem->HoverCardSlot(WidgetCardData);
}

void UW_ShopPlayerCardSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    if(!CardSubsystem)
        return;
    CardSubsystem->UnhoverCardSlot();
}
