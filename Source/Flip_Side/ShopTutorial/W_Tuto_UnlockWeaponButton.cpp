// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_UnlockWeaponButton.h"
#include "Components/Button.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

void UW_Tuto_UnlockWeaponButton::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();

    if(WeaponButton)
    {
        WeaponButton->OnClicked.AddDynamic(this, &UW_Tuto_UnlockWeaponButton::ExcuteTutorial);
    }
}


void UW_Tuto_UnlockWeaponButton::ExcuteTutorial()
{
    if(IsExecuteNextOrder)
        TutorialSubsystem->ExecuteEvents(NextOrder);
}
	
