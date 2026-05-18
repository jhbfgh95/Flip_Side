// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_CoinSlotContainer.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_CoinSlotContainer::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SlotAddButton->OnClicked.AddDynamic(this, &UW_Tuto_CoinSlotContainer::ExcuteEvent);
}


void UW_Tuto_CoinSlotContainer::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}