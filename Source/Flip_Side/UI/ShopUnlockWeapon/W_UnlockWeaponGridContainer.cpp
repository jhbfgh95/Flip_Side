// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponGridContainer.h"
#include "Components/Button.h"

void UW_UnlockWeaponGridContainer::NativeConstruct()
{
    Super::NativeConstruct();

    DealClassButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGridContainer::SetDealWUnlockGrid);

    TankClassButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGridContainer::SetTankWUnlockGrid);

    UtilClassButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGridContainer::SetUtilWUnlockGrid);

    SetDealWUnlockGrid();
}

void UW_UnlockWeaponGridContainer::SetDealWUnlockGrid()
{
    SetWeaponGrid(EWeaponClass::Deal);
}
    
void UW_UnlockWeaponGridContainer::SetTankWUnlockGrid()
{
    SetWeaponGrid(EWeaponClass::Tank);
}
    
void UW_UnlockWeaponGridContainer::SetUtilWUnlockGrid()
{
    SetWeaponGrid(EWeaponClass::Heal);
}

void UW_UnlockWeaponGridContainer::SetWeaponGrid(EWeaponClass weaponClass)
{
    
    DealWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    TankWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);

    DealClassButton->SetBackgroundColor(DefaultColor);
    TankClassButton->SetBackgroundColor(DefaultColor);
    UtilClassButton->SetBackgroundColor(DefaultColor);

    switch (weaponClass)
    {
        case EWeaponClass::Deal:
            DealClassButton->SetBackgroundColor(DealColor);
            DealWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
            break;
        
        case EWeaponClass::Tank:
            TankClassButton->SetBackgroundColor(TankColor);
            TankWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
            break;
        
        case EWeaponClass::Heal:
            UtilClassButton->SetBackgroundColor(UtilColor);
            UtilWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
            break;
        default:
            break;
    }
}