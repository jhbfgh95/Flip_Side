// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/W_CoinCreateWidget.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/Button.h"
#include "Player/GameMode_Shop.h"
#include "UI/ShopCoinCreate/W_WeaponSelectGrid.h"
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

    ChangeCoinSideButton->OnClicked.AddDynamic(this, &UW_CoinCreateWidget::ChangeCoinSide);

    dealClassGrid->InitPanelAnimation();
    tankClassGrid->InitPanelAnimation();
    utilClassGrid->InitPanelAnimation();
    CurrentOpenGrid =nullptr;
}

void UW_CoinCreateWidget::NativeDestruct()
{
    CoinCreateWSubSystem->OnCoinClassUpdate.RemoveAll(this);
    Super::NativeDestruct();
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
    if(weaponClass == EWeaponClass::None)
    {
        
        UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaa"));
        if(CurrentOpenGrid)
        {
            CurrentOpenGrid->CloseWeaponGrid();
            
            UE_LOG(LogTemp, Warning, TEXT("aaaaaa111a11aa1a"));
        }
            
        CurrentOpenGrid = nullptr;
    }

    if(weaponClass == EWeaponClass::Deal&& CurrentOpenGrid != dealClassGrid)
    {
        if(CurrentOpenGrid)
            CurrentOpenGrid->CloseWeaponGrid();
        dealClassGrid->OpenWeaponGrid();
        CurrentOpenGrid = dealClassGrid;
    }
    else if(weaponClass == EWeaponClass::Tank&& CurrentOpenGrid != tankClassGrid)
    {
        if(CurrentOpenGrid)
            CurrentOpenGrid->CloseWeaponGrid();
        tankClassGrid->OpenWeaponGrid();
        CurrentOpenGrid = tankClassGrid;
    }
    else if(weaponClass == EWeaponClass::Heal&& CurrentOpenGrid != utilClassGrid)
    {
        if(CurrentOpenGrid)
            CurrentOpenGrid->CloseWeaponGrid();
        utilClassGrid->OpenWeaponGrid();
        CurrentOpenGrid = utilClassGrid;
    }

}

void UW_CoinCreateWidget::SetDeal()
{
    CoinCreateWSubSystem->SetCoinClass(EWeaponClass::Deal);
}
void UW_CoinCreateWidget::SetTank()
{
    CoinCreateWSubSystem->SetCoinClass(EWeaponClass::Tank);
}
void UW_CoinCreateWidget::SetUtil()
{
    CoinCreateWSubSystem->SetCoinClass(EWeaponClass::Heal);
}

void UW_CoinCreateWidget::ChangeCoinSide()
{
    CoinCreateWSubSystem->ChangeCoinSide();
}