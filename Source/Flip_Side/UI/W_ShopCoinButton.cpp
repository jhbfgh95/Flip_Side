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
    
}

void UW_ShopCoinButton::CreateCoin()
{

    UE_LOG(LogTemp,Warning, TEXT("선택한 코인 클래스 : %s"),*UEnum::GetValueAsString(ShopCoinSubsystem->GetSlotCoinClass(0)));
    CoinCreateSubsystem->SelectCoin(ShopCoinSubsystem->GetSlotCoin(CoinIndex), ShopCoinSubsystem->GetSlotCoinClass(CoinIndex));
    AGameMode_Shop* gamemode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    gamemode->SetCoinCreateMode();
}

void UW_ShopCoinButton::IncreaseCoin()
{

    ShopCoinSubsystem->IncreaseSlotCoinCount(CoinIndex);
}
void UW_ShopCoinButton::DecreaseCoin()
{
    ShopCoinSubsystem->DecreaseSlotCoinCount(CoinIndex);
}

void UW_ShopCoinButton::ResetCoin()
{

    ShopCoinSubsystem->ResetCoin(CoinIndex);
}
