// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardSlotContainer.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopCard/W_ShopCardSlot.h"
#include "DataTypes/CardTypes.h"

void UW_ShopCardSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    
    CardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();

    TArray<FCardData> CardData = CardSubsystem->GetCardListArray();

    for(int i =0; i<CardData.Num(); i++)
    {
        UW_ShopCardSlot* CardSlotWidget =Cast<UW_ShopCardSlot>(CreateWidget<UUserWidget>(GetWorld(), ShopCardSlotWidget));
        if (CardSlotWidget)
        {
            ShopCardSlots.Add(CardSlotWidget);
            UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CardSlotWidget);
            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            
            CardSlotWidget->InitCardSlot(CardData[i], CardSubsystem);
        }
    }
}

void UW_ShopCardSlotContainer::NativeDestruct()
{
    Super::NativeDestruct();
}

