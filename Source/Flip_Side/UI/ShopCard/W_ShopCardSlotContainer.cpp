// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardSlotContainer.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopCard/W_ShopCardSlot.h"

void UW_ShopCardSlotContainer::InitWidget(TArray<FCardData> InCardData)
{
    for(int i =0; i<InCardData.Num(); i++)
    {
        UW_ShopCardSlot* CardSlotWidget =Cast<UW_ShopCardSlot>(CreateWidget<UUserWidget>(GetWorld(), ShopCardSlotWidget));
        if (CardSlotWidget)
        {
            ShopCardSlots.Add(CardSlotWidget);
            UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CardSlotWidget);
            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
            
            CardSlotWidget->InitCardSlot(InCardData[i]);
        }
    }
}
	
TArray<UW_ShopCardSlot*> UW_ShopCardSlotContainer::GetShopCardSlots()
{
    return ShopCardSlots;
}
