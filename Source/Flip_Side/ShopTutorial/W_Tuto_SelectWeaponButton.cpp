// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_SelectWeaponButton.h"
#include "Components/Button.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

void UW_Tuto_SelectWeaponButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();

    if(WeaponButton)
    {
        WeaponButton->OnClicked.AddDynamic(this, &UW_Tuto_SelectWeaponButton::ExcuteTutorial);
    }
    InitButton(InitID);
}


void UW_Tuto_SelectWeaponButton::ExcuteTutorial()
{
    TutorialSubsystem->ExecuteEvents(NextOrder);
}
	