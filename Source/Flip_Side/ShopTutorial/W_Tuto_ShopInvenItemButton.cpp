// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_ShopInvenItemButton.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_ShopInvenItemButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    ItemButton->OnClicked.AddDynamic(this, &UW_Tuto_ShopInvenItemButton::ExcuteEvent);
}


void UW_Tuto_ShopInvenItemButton::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}
	
