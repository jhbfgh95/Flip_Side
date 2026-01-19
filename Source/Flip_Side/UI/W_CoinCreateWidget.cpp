// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinCreateWidget.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
void UW_CoinCreateWidget::NativeConstruct()
{
    coinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
}


void UW_CoinCreateWidget::SetCreateCoinWepon(int32 WeponID)
{
    if(coinCreateWSubSystem)
    {
        //coinCreateWSubSystem->ChangeSelectedCoinWeapon(WeponID, IsCoinFront);
    }
}

void UW_CoinCreateWidget::UpdateCoinState(struct FCoinTypeStructure UpdateCoinInfo)
{
    
}

//코드가 안 예쁨 수정해야할듯 
void UW_CoinCreateWidget::SetClassGrid(EWeaponClass weaponClass)
{
    switch (weaponClass)
    {
    case EWeaponClass::Deal:
        dealClassGrid->SetVisibility(ESlateVisibility::Visible);
        break;
    case EWeaponClass::Tank:
        tankClassGrid->SetVisibility(ESlateVisibility::Visible);
        break;
    case EWeaponClass::Heal:
        utilClassGrid->SetVisibility(ESlateVisibility::Visible);
        break;
    
    default:
        break;
    }
}