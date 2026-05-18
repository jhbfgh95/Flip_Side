// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_UnlockWeaponButton.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_UnlockWeaponButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    WeaponButton->OnClicked.AddDynamic(this, &UW_Tuto_UnlockWeaponButton::ExcuteEvent);
    InitButton(InitID);
}


void UW_Tuto_UnlockWeaponButton::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}
	