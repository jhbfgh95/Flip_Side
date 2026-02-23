// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_UnlockWeaponWidget.h"
#include "Components/Button.h"
#include "UI/W_UnlockWeaponGrid.h"
#include "Subsystem/ShopUnlockWeaponWSubsystem.h"
void UW_UnlockWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();

    DealWGridButton ->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::SetDealWUnlockGrid);	
	
    TankWGridButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::SetTankWUnlockGrid);	
	
    UtilWGridButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::SetUtilWUnlockGrid);	

    UnlockWeaponButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponWidget::ClickUnlockWeaponButton);
    HideAllUnlockGrid();
    DealWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
}

void UW_UnlockWeaponWidget::SetDealWUnlockGrid()
{
    if(DealWUnlockGrid->GetVisibility() != ESlateVisibility::Visible)
    {
        HideAllUnlockGrid();
        DealWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
    }

}
    
void UW_UnlockWeaponWidget::SetTankWUnlockGrid()
{
    if(TankWUnlockGrid->GetVisibility()!= ESlateVisibility::Visible)
    {
        HideAllUnlockGrid();
        TankWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
    }
}
    
void UW_UnlockWeaponWidget::SetUtilWUnlockGrid()
{
    if(UtilWUnlockGrid->GetVisibility()!= ESlateVisibility::Visible)
    {
        HideAllUnlockGrid();
        UtilWUnlockGrid->SetVisibility(ESlateVisibility::Visible);
    }
}

void UW_UnlockWeaponWidget::HideAllUnlockGrid()
{
    DealWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    TankWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
    UtilWUnlockGrid->SetVisibility(ESlateVisibility::Collapsed);
}


void UW_UnlockWeaponWidget::ClickUnlockWeaponButton()
{
    UE_LOG(LogTemp, Warning, TEXT("무기 해금 버튼 누름"));
    UShopUnlockWeaponWSubsystem* subsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    subsystem->UnlockCurrentWeapon();
}