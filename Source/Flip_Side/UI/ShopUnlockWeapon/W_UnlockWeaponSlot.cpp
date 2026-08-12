// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UW_UnlockWeaponSlot::NativeConstruct()
{
    Super::NativeConstruct();
    SelectButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponSlot::ClickSlot);
    SelectButton->OnHovered.AddDynamic(this, &UW_UnlockWeaponSlot::HoverSlot);
    SelectButton->OnUnhovered.AddDynamic(this, &UW_UnlockWeaponSlot::UnhoverSlot);
}

void UW_UnlockWeaponSlot::InitWidget(FFaceData InitWeaponData, UShopUnlockWeaponWSubsystem* InitSubsystem)
{
    UnlockWeaponData = InitWeaponData;
    UnlockWeaponSubsystem = InitSubsystem;
}


void UW_UnlockWeaponSlot::ClickSlot()
{
    UnlockWeaponSubsystem->SelectUnlockWeapon(UnlockWeaponData.WeaponID);
}

void UW_UnlockWeaponSlot::HoverSlot()
{
    UnlockWeaponSubsystem->SelectUnlockWeapon(UnlockWeaponData.WeaponID);
}
	
void UW_UnlockWeaponSlot::UnhoverSlot()
{
    UnlockWeaponSubsystem->UnSelectWeapon();
}