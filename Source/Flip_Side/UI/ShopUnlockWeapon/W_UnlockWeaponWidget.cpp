// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "Components/Button.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlotContainer.h"
#include "UI/ShopUnlockWeapon/W_UnlockSelectWeaponButton.h"
#include "UI/W_WeaponDescription.h"
void UW_UnlockWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_UnlockWeaponWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

UW_UnlockWeaponSlotContainer* UW_UnlockWeaponWidget::GetUnlockWeaponSlotContainer() const
{
    return UnlockWeaponSlotContainer;
}

UW_PriceWidget* UW_UnlockWeaponWidget::GetWeaponPriceWidget() const
{
    return WeaponPriceText;
}

UW_WeaponDescription* UW_UnlockWeaponWidget::GetWeaponDescription() const
{
    return WeaponDes;
}
