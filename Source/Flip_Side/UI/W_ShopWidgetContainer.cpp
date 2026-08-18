// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopWidgetContainer.h"
#include "Player/GameMode_Shop.h"
#include "UI/ShopItem/W_ShopItemWidget.h"
#include "UI/ShopCard/W_ShopCardMainWidget.h"
#include "UI/ShopCoinManage/W_ShopCoinWidget.h"

void UW_ShopWidgetContainer::NativeConstruct()
{
    Super::NativeConstruct();

    ShopGameMode= Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    ShopGameMode->OnCoinManageMode.AddDynamic(this, &UW_ShopWidgetContainer::SetShopCoinWidget);
    ShopGameMode->OnShopItemMode.AddDynamic(this, &UW_ShopWidgetContainer::SetShopItemWidget);
    ShopGameMode->OnSelectCardMode.AddDynamic(this, &UW_ShopWidgetContainer::SetShopCardWidget);
    ShopGameMode->OnUnlockWeaponMode.AddDynamic(this, &UW_ShopWidgetContainer::SetShopUnlockWeaponWidget);
    ShopGameMode->OnCheckBossMode.AddDynamic(this, & UW_ShopWidgetContainer::SetShopBossWidget);

    HideAllWidget();
}

void UW_ShopWidgetContainer::SetShopItemWidget()
{
    SetWidget(ShopItemWidget);
}


void UW_ShopWidgetContainer::SetShopUnlockWeaponWidget()
{
    SetWidget(ShopUnlockWeaponWidget);
}   


void UW_ShopWidgetContainer::SetShopCoinWidget()
{
    SetWidget(ShopCoinWidget);
}


void UW_ShopWidgetContainer::SetShopCardWidget()
{
    SetWidget(ShopCardWidget);
}

void UW_ShopWidgetContainer::SetShopBossWidget()
{
    SetWidget(ShopBossWidget);
}	
	
void UW_ShopWidgetContainer::HideAllWidget()
{
    ShopItemWidget->SetVisibility(ESlateVisibility::Collapsed);
    ShopUnlockWeaponWidget->SetVisibility(ESlateVisibility::Collapsed);
    //ShopCoinWidget->SetVisibility(ESlateVisibility::Collapsed);
    ShopCardWidget->SetVisibility(ESlateVisibility::Collapsed);
    ShopBossWidget->SetVisibility(ESlateVisibility::Collapsed);
}


void UW_ShopWidgetContainer::SetWidget(UUserWidget* HideWidget)
{
    HideAllWidget();
    HideWidget->SetVisibility(ESlateVisibility::Visible);
}
	

UW_ShopItemWidget* UW_ShopWidgetContainer::GetShopItemWidget()
{
    return ShopItemWidget;
}

UW_ShopCardMainWidget* UW_ShopWidgetContainer::GetShopCardWidget()
{
    return ShopCardWidget;
}
	
UW_ShopCoinWidget* UW_ShopWidgetContainer::GetShopCoinWidget()
{
    return ShopCoinWidget;
}