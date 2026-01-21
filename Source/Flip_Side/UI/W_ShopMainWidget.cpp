// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopMainWidget.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"
void UW_ShopMainWidget::NativeConstruct()
{
    Super::NativeConstruct();
    shopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    CoinCreateButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickCoinCreateButton);
}


void UW_ShopMainWidget::ClickCoinCreateButton()
{
    if(shopGameMode)
    {
        shopGameMode->SetCoinManageMode();
    }
}