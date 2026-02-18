// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopMainWidget.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"
void UW_ShopMainWidget::NativeConstruct()
{
    Super::NativeConstruct();
    shopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    CoinCreateButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickCoinCreateButton);
    ShopItemButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickShopItemButton);
    CardSelectButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickCardSelectButton);
    UnlockWeaponButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickUnlockWeaponButton);
}


void UW_ShopMainWidget::ClickCoinCreateButton()
{
    if(shopGameMode)
    {
        shopGameMode->SetCoinManageMode();
    }
}


void UW_ShopMainWidget::ClickShopItemButton()
{
    if(shopGameMode)
    {
        shopGameMode->SetShopItemMode();
    }
}


void UW_ShopMainWidget::ClickCardSelectButton()
{
    if(shopGameMode)
    {
        shopGameMode->SetCardSelectMode();
    }
}

void UW_ShopMainWidget::ClickUnlockWeaponButton()
{
    if(shopGameMode)
    {
        shopGameMode->SetUnlockWeaponMode();
    }
}