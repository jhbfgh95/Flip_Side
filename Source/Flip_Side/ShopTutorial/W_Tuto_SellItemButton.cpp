// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_SellItemButton.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_SellItemButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    ItemButton->OnClicked.AddDynamic(this, &UW_Tuto_SellItemButton::ExcuteEvent);
}

void UW_Tuto_SellItemButton::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}

