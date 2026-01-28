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
}


void UW_CoinManageWidget::SelectCoin()
{
    CoinCreateSubsystem->SelectCoin(ShopCoinSubsystem->GetSlotCoin(0));

    AGameMode_Shop* gamemode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    gamemode->SetCoinCreateMode();
}