// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopWidgetContainer.h"
#include "Components/WidgetSwitcher.h"
#include "UI/ShopItem/W_ShopItemWidget.h"
#include "UI/ShopCard/W_ShopCardMainWidget.h"
#include "UI/ShopCoinManage/W_ShopCoinWidget.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "UI/W_ShopCheckStartGame.h"
#include "Player/GameMode_Shop.h"

void UW_ShopWidgetContainer::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    ShopNavigationBar->OnShopPageRequested.AddDynamic(
        this, &UW_ShopWidgetContainer::HandleShopPageRequested);
    ShopCheckStartGameWidget->OnStartGameConfirmed.AddDynamic(
        this, &UW_ShopWidgetContainer::HandleStartGameConfirmed);

    HideShopContent();
}

void UW_ShopWidgetContainer::HandleShopPageRequested(EShopPage Page)
{
	OnShopPageRequested.Broadcast(Page);
}

void UW_ShopWidgetContainer::HideShopContent()
{
	ShopContentSwitcher->SetVisibility(ESlateVisibility::Hidden);
}

void UW_ShopWidgetContainer::ShowShopPage(EShopPage Page)
{
	switch(Page)
	{
	case EShopPage::Main:
		ShopContentSwitcher->SetActiveWidget(ShopMainWidget);
		break;
	case EShopPage::Coin:
		ShopContentSwitcher->SetActiveWidget(ShopCoinWidget);
		break;
	case EShopPage::Item:
		ShopContentSwitcher->SetActiveWidget(ShopItemWidget);
		break;
	case EShopPage::Card:
		ShopContentSwitcher->SetActiveWidget(ShopCardWidget);
		break;
	case EShopPage::UnlockWeapon:
		ShopContentSwitcher->SetActiveWidget(ShopUnlockWeaponWidget);
		break;
	case EShopPage::Boss:
		ShopContentSwitcher->SetActiveWidget(ShopBossWidget);
		break;
	case EShopPage::GameStart:
	{
		UE_LOG(LogTemp, Warning, TEXT("게임시작?"));
		AGameMode_Shop* ShopGameMode = GetWorld()->GetAuthGameMode<AGameMode_Shop>();
		const bool bHasCoin = IsValid(ShopGameMode) && ShopGameMode->CheckHaveCoin();
		ShopCheckStartGameWidget->SetCheckStartGameText(bHasCoin);
		ShopContentSwitcher->SetActiveWidget(ShopCheckStartGameWidget);
		break;
	}
	default:
		return;
	}

	ShopContentSwitcher->SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopWidgetContainer::HandleStartGameConfirmed()
{
	if (AGameMode_Shop* ShopGameMode = GetWorld()->GetAuthGameMode<AGameMode_Shop>())
	{
		ShopGameMode->ChangeBattleLevel();
	}
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

UW_UnlockWeaponWidget* UW_ShopWidgetContainer::GetShopUnlockWeaponWidget()
{
    return ShopUnlockWeaponWidget;
}

UW_ShopCheckStartGame* UW_ShopWidgetContainer::GetShopCheckStartWidget()
{
	return ShopCheckStartGameWidget;
}