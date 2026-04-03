// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "Components/Button.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponGrid.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
void UW_UnlockWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    UnlockWeaponSubsystem->OnChangeUnlockWaeponClass.AddDynamic(this, &UW_UnlockWeaponWidget::SetUnlockWeaponGrid);

    CurrentUnlockGrid = nullptr;
    DealWUnlockGrid->InitPanelAnimation();
    TankWUnlockGrid->InitPanelAnimation();
    UtilWUnlockGrid->InitPanelAnimation();

}

void UW_UnlockWeaponWidget::NativeDestruct()
{
    UnlockWeaponSubsystem->OnChangeUnlockWaeponClass.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_UnlockWeaponWidget::SetDealWUnlockGrid()
{

    if(CurrentUnlockGrid&&CurrentUnlockGrid != DealWUnlockGrid)
    {
        CurrentUnlockGrid->CloseUnlockPanel();
        DealWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=DealWUnlockGrid;
    }

    if(!CurrentUnlockGrid)
    {
        DealWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=DealWUnlockGrid;
    }
}
    
void UW_UnlockWeaponWidget::SetTankWUnlockGrid()
{
    if(CurrentUnlockGrid&&CurrentUnlockGrid != TankWUnlockGrid)
    {
        CurrentUnlockGrid->CloseUnlockPanel();
        TankWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=TankWUnlockGrid;
    }
    if(!CurrentUnlockGrid)
    {
        TankWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=TankWUnlockGrid;
    }
}
    
void UW_UnlockWeaponWidget::SetUtilWUnlockGrid()
{
    if(CurrentUnlockGrid&&CurrentUnlockGrid != UtilWUnlockGrid)
    {
        CurrentUnlockGrid->CloseUnlockPanel();
        UtilWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=UtilWUnlockGrid;
    }

    if(!CurrentUnlockGrid)
    {
        UtilWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=UtilWUnlockGrid;
    }
}

void UW_UnlockWeaponWidget::HideAllUnlockGrid()
{
    DealWUnlockGrid->CloseUnlockPanel();
    TankWUnlockGrid->CloseUnlockPanel();
    UtilWUnlockGrid->CloseUnlockPanel();
}

void UW_UnlockWeaponWidget::SetUnlockWeaponGrid(EWeaponClass weaponClass)
{
    switch (weaponClass)
    {
        case EWeaponClass::Tank:
            SetTankWUnlockGrid();
            break;
        case EWeaponClass::Deal:
            SetDealWUnlockGrid();
            break;
        case EWeaponClass::Heal:
            SetUtilWUnlockGrid();
            break;
        default:
            break;
    }
}