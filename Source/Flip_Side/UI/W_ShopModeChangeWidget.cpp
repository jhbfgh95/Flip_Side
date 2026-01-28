// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopModeChangeWidget.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"

void UW_ShopModeChangeWidget::NativeConstruct()
{
    Super::NativeConstruct();
    ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    //CoinCreateButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickCoinCreateButton);
    //ShopItemButton->OnClicked.AddDynamic(this,&UW_ShopMainWidget::ClickShopItemButton);

    WidgetOpenButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::OpenWidget);
	CoinCreateModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickCoinCreateMode);
	CardSelectModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickCardSelecrMode);
	ShopItemModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickShopItemMode);
	CheckBossModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickCheckBossMode);
}

void UW_ShopModeChangeWidget::ClickCoinCreateMode()
{
    if(ShopGameMode)
    {
        ShopGameMode->SetCoinManageMode();
    }
}

void UW_ShopModeChangeWidget::ClickCardSelecrMode()
{
    if(ShopGameMode)
    {
        ShopGameMode->SetCardSelectMode();
    }
}
void UW_ShopModeChangeWidget::ClickShopItemMode()
{
    if(ShopGameMode)
    {
        ShopGameMode->SetShopItemMode();
    }
}
void UW_ShopModeChangeWidget::ClickCheckBossMode()
{
    if(ShopGameMode)
    {
        //ShopGameMode->SetCheckBossMode();
    }
}
void UW_ShopModeChangeWidget::OpenWidget()
{

}