// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_CoinCountButton.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_CoinCountButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    CountButton->OnClicked.AddDynamic(this, &UW_Tuto_CoinCountButton::ExcuteEvent);
}


void UW_Tuto_CoinCountButton::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}
	
