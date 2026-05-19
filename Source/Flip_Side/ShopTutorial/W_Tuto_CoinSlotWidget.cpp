// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_CoinSlotWidget.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_CoinSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SlotButton->OnClicked.AddDynamic(this, &UW_Tuto_CoinSlotWidget::ExcuteEvent);
}


void UW_Tuto_CoinSlotWidget::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}

