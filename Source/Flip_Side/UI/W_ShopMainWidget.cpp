// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopMainWidget.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"
void UW_ShopMainWidget::NativeConstruct()
{
    Super::NativeConstruct();
    shopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    ShopItemButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickShopItemButton);
    CardSelectButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickCardSelectButton);
    UnlockWeaponButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickUnlockWeaponButton);

    GameStartButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickBattleStartButton);

    
}


void UW_ShopMainWidget::ClickCoinManageButton()
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


void UW_ShopMainWidget::ClickBattleStartButton()
{
    if(shopGameMode)
    {
        shopGameMode->ChangeBattleLevel();
    }
}