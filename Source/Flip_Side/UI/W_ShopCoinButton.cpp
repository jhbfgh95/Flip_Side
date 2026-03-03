// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopCoinButton.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"
void UW_ShopCoinButton::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

    CoinButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::CreateCoin);
	CoinAddButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::IncreaseCoin);

    CoinReduceButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::DecreaseCoin);

    CoindistroyButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::ResetCoin);

    
    SlotUpButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::ChangeSlotUp);

	
    SlotDownButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::ChangeSlotDown);
	
	
    UnlockCoinSlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinButton::UnlockCoinSlot);
    
}

void UW_ShopCoinButton::CreateCoin()
{
    //UE_LOG(LogTemp,Warning, TEXT("선택한 코인 클래스 : %s"),*UEnum::GetValueAsString(ShopCoinSubsystem->GetCurrentSlotCoinClass()));
    CoinCreateSubsystem->SelectCoin(ShopCoinSubsystem->GetCurrentSlotCoin(), ShopCoinSubsystem->GetCurrentSlotCoinClass());
    AGameMode_Shop* gamemode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    gamemode->SetCoinCreateMode();
}

void UW_ShopCoinButton::IncreaseCoin()
{
    //ShopCoinSubsystem->IncreaseSlotCoinCount(CoinIndex);
    ShopCoinSubsystem->IncreaseSlotCoinCount();
}
void UW_ShopCoinButton::DecreaseCoin()
{
    //ShopCoinSubsystem->DecreaseSlotCoinCount(CoinIndex);
    ShopCoinSubsystem->DecreaseSlotCoinCount();
}

void UW_ShopCoinButton::ResetCoin()
{
    //ShopCoinSubsystem->ChangeCoinSlotRight();
   // ShopCoinSubsystem->ResetCoin(CoinIndex);
}

void UW_ShopCoinButton::ChangeSlotUp()
{
    ShopCoinSubsystem->ChangeCoinSlotRight();

}

void UW_ShopCoinButton::ChangeSlotDown()
{
    ShopCoinSubsystem->ChangeCoinSlotLeft();
}
	
void UW_ShopCoinButton::UnlockCoinSlot()
{
    ShopCoinSubsystem->UnlockCurrentCoinSlot();
}