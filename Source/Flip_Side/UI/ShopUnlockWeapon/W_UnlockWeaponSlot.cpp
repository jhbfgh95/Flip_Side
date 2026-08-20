// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"
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

void UW_UnlockWeaponSlot::InitWidget(const FFaceData& InitWeaponData)
{
    UnlockWeaponData = InitWeaponData;
    WeaponImage->SetBrushFromTexture(UnlockWeaponData.WeaponIcon);
    WeaponName->SetText(FText::FromString(UnlockWeaponData.WeaponName));
}


void UW_UnlockWeaponSlot::ClickSlot()
{
    OnClickedUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);
}

void UW_UnlockWeaponSlot::HoverSlot()
{
    OnHoveredUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);
}
	
void UW_UnlockWeaponSlot::UnhoverSlot()
{
    OnUnhoveredUnlockWeaponSlot.Broadcast();
}
