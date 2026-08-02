// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_WeaponGridContainer.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "UI/ShopCoinCreate/W_WeaponSelectGrid.h"
void UW_WeaponGridContainer::NativeConstruct()
{
    Super::NativeConstruct();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

    CoinSubsystem->OnCoinSlotChange.AddDynamic(this, &UW_WeaponGridContainer::SetUnlockImageSlotChange);
    CoinSubsystem->OnChangeCoinSlotCount.AddDynamic(this, &UW_WeaponGridContainer::SetUnlockImage);
    
    DealGridButton->OnClicked.AddDynamic(this, &UW_WeaponGridContainer::SetDealGrid);
    TankGridButton->OnClicked.AddDynamic(this, &UW_WeaponGridContainer::SetTankGrid);
    UtilGridButton->OnClicked.AddDynamic(this, &UW_WeaponGridContainer::SetUtilGrid);

    SetDealGrid();
}
void UW_WeaponGridContainer::NativeDestruct()
{
    CoinSubsystem->OnCoinSlotChange.RemoveAll(this);
    CoinSubsystem->OnChangeCoinSlotCount.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_WeaponGridContainer::SetUnlockImageSlotChange()
{
}   

void UW_WeaponGridContainer::SetUnlockImage(bool IsIncrease)
{
    /*
    if(CoinSubsystem->GetCurrentSlotCount()==0)
    {
        LockImage->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        LockImage->SetVisibility(ESlateVisibility::Collapsed);
    }*/
}   

void UW_WeaponGridContainer::SetDealGrid()
{
    SetWeaponGrid(EWeaponClass::Deal);
}

void UW_WeaponGridContainer::SetTankGrid()
{
    SetWeaponGrid(EWeaponClass::Tank);
}

void UW_WeaponGridContainer::SetUtilGrid()
{
    SetWeaponGrid(EWeaponClass::Heal);
}

void UW_WeaponGridContainer::SetWeaponGrid(EWeaponClass weaponClass)
{
    /*
    DealGrid->SetVisibility(ESlateVisibility::Collapsed);
    TankGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilGrid->SetVisibility(ESlateVisibility::Collapsed);

    DealGridButton->SetBackgroundColor(DefaultColor);
    TankGridButton->SetBackgroundColor(DefaultColor);
    UtilGridButton->SetBackgroundColor(DefaultColor);

    switch (weaponClass)
    {
        case EWeaponClass::Deal:
            DealGridButton->SetBackgroundColor(DealColor);
            DealGrid->SetVisibility(ESlateVisibility::Visible);
            break;
        
        case EWeaponClass::Tank:
            TankGridButton->SetBackgroundColor(TankColor);
            TankGrid->SetVisibility(ESlateVisibility::Visible);
            break;
        
        case EWeaponClass::Heal:
            UtilGridButton->SetBackgroundColor(UtilColor);
            UtilGrid->SetVisibility(ESlateVisibility::Visible);
            break;
        default:
            break;
    }

    CoinSubsystem->ChangeCoinClass(weaponClass);*/
}