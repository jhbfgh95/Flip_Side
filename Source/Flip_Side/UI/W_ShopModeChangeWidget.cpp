// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopModeChangeWidget.h"
#include "Player/GameMode_Shop.h"
#include "Components/Button.h"
#include "Styling/SlateBrush.h"

void UW_ShopModeChangeWidget::NativeConstruct()
{
    Super::NativeConstruct();
    ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    WidgetOpenButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::OpenWidget);
	CoinCreateModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickCoinCreateMode);
	CardSelectModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickCardSelecrMode);
	ShopItemModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickShopItemMode);
	CheckBossModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickCheckBossMode);
    ShopMainModeButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickShopMainModeButton);
    UnlockWeaponButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickUnlockWeaponMode);
    UnlockCardButton->OnClicked.AddDynamic(this,&UW_ShopModeChangeWidget::ClickUnlockCardMode);

    

}

void UW_ShopModeChangeWidget::NativeDestruct()
{
    
}

void UW_ShopModeChangeWidget::SetButtonTexture(UTexture2D* SetButtonTexture)
{
    FButtonStyle ButtonStyle = WidgetOpenButton->GetStyle();
    FSlateBrush Brush;
    Brush.SetResourceObject(SetButtonTexture);
    ButtonStyle.SetNormal(Brush);
    ButtonStyle.SetHovered(Brush);
    ButtonStyle.SetPressed(Brush);
    WidgetOpenButton->SetStyle(ButtonStyle);
}

void UW_ShopModeChangeWidget::ClickCoinCreateMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetCoinManageMode();
        SetButtonTexture(T_CoinManageMode);
    }
}

void UW_ShopModeChangeWidget::ClickCardSelecrMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetCardSelectMode();
        SetButtonTexture(T_CardSeletMode);
    }
}
void UW_ShopModeChangeWidget::ClickShopItemMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetShopItemMode();
        SetButtonTexture(T_ShopItemMode);
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
        SetButtonTexture(T_UnlockWeaponMode);
    }
}

void UW_ShopModeChangeWidget::ClickUnlockCardMode()
{
    OpenWidget();
    if(ShopGameMode)
    {
        ShopGameMode->SetUnlockCardMode();
        SetButtonTexture(T_UnlockCardMode);
    }
}  