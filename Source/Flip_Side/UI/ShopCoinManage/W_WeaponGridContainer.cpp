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
    CoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &UW_WeaponGridContainer::SetUnlockImage);
    
    DealGridButton->OnClicked.AddDynamic(this, &UW_WeaponGridContainer::SetDealGrid);
    TankGridButton->OnClicked.AddDynamic(this, &UW_WeaponGridContainer::SetTankGrid);
    UtilGridButton->OnClicked.AddDynamic(this, &UW_WeaponGridContainer::SetUtilGrid);
}
void UW_WeaponGridContainer::NativeDestruct()
{
    CoinSubsystem->OnCoinSlotChange.RemoveAll(this);
    CoinSubsystem->OnUnlockCoinSlot.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_WeaponGridContainer::SetUnlockImageSlotChange(bool IsChangeBottom)
{
    SetUnlockImage();
}   

void UW_WeaponGridContainer::SetUnlockImage()
{
    if(CoinSubsystem->GetCurrentCoinUnlock())
    {
        LockImage->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        UE_LOG(LogTemp,Warning, TEXT("ssssss"));
        LockImage->SetVisibility(ESlateVisibility::Visible);
    }
}   

void UW_WeaponGridContainer::SetDealGrid()
{
    
    DealGrid->SetVisibility(ESlateVisibility::Visible);
    TankGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilGrid->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_WeaponGridContainer::SetTankGrid()
{
    DealGrid->SetVisibility(ESlateVisibility::Collapsed);
    TankGrid->SetVisibility(ESlateVisibility::Visible);
    UtilGrid->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_WeaponGridContainer::SetUtilGrid()
{
    DealGrid->SetVisibility(ESlateVisibility::Collapsed);
    TankGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilGrid->SetVisibility(ESlateVisibility::Visible);

}