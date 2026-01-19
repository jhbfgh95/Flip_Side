// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinCreateWidget.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
void UW_CoinCreateWidget::NativeConstruct()
{
    Super::NativeConstruct();
    CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
    if(CoinCreateWSubSystem)
	{
		//코인클래스 변경됬을때
		CoinCreateWSubSystem->OnCoinClassUpdate.AddDynamic(this, &UW_CoinCreateWidget::SetClassGrid);
	}


}


void UW_CoinCreateWidget::SetCreateCoinWepon(int32 WeponID)
{
    if(CoinCreateWSubSystem)
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
    UE_LOG(LogTemp, Warning, TEXT("값설정됨"));

    switch (weaponClass)
    {
    case EWeaponClass::Deal:
        dealClassGrid->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Warning, TEXT("딜"));
        break;
    case EWeaponClass::Tank:
        tankClassGrid->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Warning, TEXT("텡"));
        break;
    case EWeaponClass::Heal:
        utilClassGrid->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Warning, TEXT("힐"));
        break;
    
    default:
        break;
    }
}