// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "Components/Button.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponGrid.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
void UW_UnlockWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();

    DealWGridButton ->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::SetDealWUnlockGrid);	
	
    TankWGridButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::SetTankWUnlockGrid);	
	
    UtilWGridButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::SetUtilWUnlockGrid);	

    UnlockWeaponButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::ClickUnlockWeaponButton);
    
    CurrentUnlockGrid = DealWUnlockGrid;
    
    TankWUnlockGrid->InitPanelAnimation();
    UtilWUnlockGrid->InitPanelAnimation();
}
void UW_UnlockWeaponWidget::SetDealWUnlockGrid()
{
    if(CurrentUnlockGrid != DealWUnlockGrid)
    {
        CurrentUnlockGrid->CloseUnlockPanel();
        DealWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=DealWUnlockGrid;
    }

}
    
void UW_UnlockWeaponWidget::SetTankWUnlockGrid()
{
    if(CurrentUnlockGrid != TankWUnlockGrid)
    {
        CurrentUnlockGrid->CloseUnlockPanel();
        TankWUnlockGrid->OpenUnlockPanel();
        CurrentUnlockGrid=TankWUnlockGrid;
    }
}
    
void UW_UnlockWeaponWidget::SetUtilWUnlockGrid()
{
    if(CurrentUnlockGrid != UtilWUnlockGrid)
    {
        CurrentUnlockGrid->CloseUnlockPanel();
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


void UW_UnlockWeaponWidget::ClickUnlockWeaponButton()
{
    UE_LOG(LogTemp, Warning, TEXT("무기 해금 버튼 누름"));
    UShopUnlockWeaponWSubsystem* subsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    subsystem->UnlockCurrentWeapon();
}