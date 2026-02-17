// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinCreateWidget.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/Button.h"
#include "Player/GameMode_Shop.h"
void UW_CoinCreateWidget::NativeConstruct()
{
    Super::NativeConstruct();
    CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
    ShopCoinSubSystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    if(CoinCreateWSubSystem)
	{
		//코인클래스 변경됬을때
		CoinCreateWSubSystem->OnCoinClassUpdate.AddDynamic(this, &UW_CoinCreateWidget::SetClassGrid);
	}

    FinishButton->OnClicked.AddDynamic(this, &UW_CoinCreateWidget::FinishCreate);

    TestClass->OnClicked.AddDynamic(this, &UW_CoinCreateWidget::SetDeal);
    
    TestClass2->OnClicked.AddDynamic(this, &UW_CoinCreateWidget::SetTank);
    
    TestClass3->OnClicked.AddDynamic(this, &UW_CoinCreateWidget::SetUtil);
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

void UW_CoinCreateWidget::FinishCreate()
{
    ShopCoinSubSystem->SetSlotCoin(CoinCreateWSubSystem->GetSelectCoin(), CoinCreateWSubSystem->GetSelectCoinClass());
    ShopGameMode->SetCoinManageMode();
}

//코드가 안 예쁨 수정해야할듯 
void UW_CoinCreateWidget::SetClassGrid(EWeaponClass weaponClass)
{
    FinishButton->SetVisibility(ESlateVisibility::Visible);
    dealClassGrid->SetVisibility(ESlateVisibility::Collapsed);
    utilClassGrid->SetVisibility(ESlateVisibility::Collapsed);
    tankClassGrid->SetVisibility(ESlateVisibility::Collapsed);
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

void UW_CoinCreateWidget::SetDeal()
{
    CoinCreateWSubSystem->SetCoinClass(EWeaponClass::Deal);
    CoinCreateWSubSystem->OffClassSelectMode();
}
void UW_CoinCreateWidget::SetTank()
{
    CoinCreateWSubSystem->SetCoinClass(EWeaponClass::Tank);
    CoinCreateWSubSystem->OffClassSelectMode();
}
void UW_CoinCreateWidget::SetUtil()
{
    CoinCreateWSubSystem->SetCoinClass(EWeaponClass::Heal);
    CoinCreateWSubSystem->OffClassSelectMode();
}
