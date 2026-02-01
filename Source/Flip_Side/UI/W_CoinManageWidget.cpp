// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinManageWidget.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"
void UW_CoinManageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

    CoinSelectButton->OnClicked.AddDynamic(this, &UW_CoinManageWidget::SelectCoin);
    
    CoinSelectButton2->OnClicked.AddDynamic(this, &UW_CoinManageWidget::SelectCoin2);
}


void UW_CoinManageWidget::SelectCoin()
{
    //CoinCreateSubsystem->SelectCoin(ShopCoinSubsystem->GetSlotCoin(0));

    UE_LOG(LogTemp,Warning, TEXT("선택한 코인 클래스 : %s"),*UEnum::GetValueAsString(ShopCoinSubsystem->GetSlotCoinClass(0)));
    CoinCreateSubsystem->SelectCoin(ShopCoinSubsystem->GetSlotCoin(0), ShopCoinSubsystem->GetSlotCoinClass(0));
    AGameMode_Shop* gamemode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    gamemode->SetCoinCreateMode();
}

void UW_CoinManageWidget::SelectCoin2()
{
    UE_LOG(LogTemp,Warning, TEXT("선택한 코인 클래스 : %s"),*UEnum::GetValueAsString(ShopCoinSubsystem->GetSlotCoinClass(1)));
    CoinCreateSubsystem->SelectCoin(ShopCoinSubsystem->GetSlotCoin(1), ShopCoinSubsystem->GetSlotCoinClass(1));
    AGameMode_Shop* gamemode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    gamemode->SetCoinCreateMode();


}
