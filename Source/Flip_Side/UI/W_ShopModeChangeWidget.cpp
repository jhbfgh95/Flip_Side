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
    ShopMainModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickShopMainModeButton);
    UnlockWeaponButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickUnlockWeaponMode);
}

void UW_ShopModeChangeWidget::NativeDestruct()
{

}
void UW_ShopModeChangeWidget::ClickCoinCreateMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetCoinManageMode();
    }
}

void UW_ShopModeChangeWidget::ClickCardSelecrMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetCardSelectMode();
    }
}
void UW_ShopModeChangeWidget::ClickShopItemMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetShopItemMode();
    }
}
void UW_ShopModeChangeWidget::ClickCheckBossMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        //ShopGameMode->SetCheckBossMode();
    }
}
void UW_ShopModeChangeWidget::OpenWidget()
{
    if(IsSlidePanelOpen)
    {
        IsSlidePanelOpen = false;
        PlayAnimation(SlidePanelCloseAnim);
    }
    else
    {
        IsSlidePanelOpen = true;
        PlayAnimation(SlidePanelOpenAnim);
    }
}

void UW_ShopModeChangeWidget::ClickShopMainModeButton()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetShopMainMode();
    }
}


void UW_ShopModeChangeWidget::ClickUnlockWeaponMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetUnlockWeaponMode();
    }
}