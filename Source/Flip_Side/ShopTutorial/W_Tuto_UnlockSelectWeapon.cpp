// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_UnlockSelectWeapon.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_UnlockSelectWeapon::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    UnlockButton->OnClicked.AddDynamic(this, &UW_Tuto_UnlockSelectWeapon::ExcuteEvent);
}


void UW_Tuto_UnlockSelectWeapon::ExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}
	