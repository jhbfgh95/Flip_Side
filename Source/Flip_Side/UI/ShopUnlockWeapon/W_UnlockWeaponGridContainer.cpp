// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponGridContainer.h"
#include "Components/Button.h"

void UW_UnlockWeaponGridContainer::NativeConstruct()
{
    Super::NativeConstruct();

    DealClassButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGridContainer::SetDealWUnlockGrid);

    TankClassButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGridContainer::SetTankWUnlockGrid);

    UtilClassButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGridContainer::SetUtilWUnlockGrid);
}

void UW_UnlockWeaponGridContainer::SetDealWUnlockGrid()
{
    DealWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
    TankWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
}
    
void UW_UnlockWeaponGridContainer::SetTankWUnlockGrid()
{
    TankWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
    DealWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
}
    
void UW_UnlockWeaponGridContainer::SetUtilWUnlockGrid()
{
    UtilWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
    DealWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    TankWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
}