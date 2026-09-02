// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockSelectWeaponButton.h"
#include "Components/Button.h"

void UW_UnlockSelectWeaponButton::NativeConstruct()
{
    Super::NativeConstruct();
    UnlockButton->OnClicked.AddDynamic(this, &UW_UnlockSelectWeaponButton::UnlockSelectWeapon);
}
	
void UW_UnlockSelectWeaponButton::UnlockSelectWeapon()
{
    OnClickedUnlockSelectedWeapon.Broadcast();
}

void UW_UnlockSelectWeaponButton::ShowButton()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UW_UnlockSelectWeaponButton::HideButton()
{
    SetVisibility(ESlateVisibility::Collapsed);
}
