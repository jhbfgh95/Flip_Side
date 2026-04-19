// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockSelectWeaponButton.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Components/Button.h"

void UW_UnlockSelectWeaponButton::NativeConstruct()
{
    Super::NativeConstruct();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    UnlockButton->OnClicked.AddDynamic(this, &UW_UnlockSelectWeaponButton::UnlockSelectWeapon);
}
	
void UW_UnlockSelectWeaponButton::UnlockSelectWeapon()
{
    UnlockWeaponSubsystem->UnlockCurrentWeapon();
}