#include "UI/W_ShopNavigationBar.h"

#include "Components/Button.h"

void UW_ShopNavigationBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MainButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowMainPage);
	CoinButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowCoinPage);
	ItemButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowItemPage);
	CardButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowCardPage);
	UnlockWeaponButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowUnlockWeaponPage);
	BossButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowBossPage);
}

void UW_ShopNavigationBar::ShowCoinPage()
{
	OnShopPageRequested.Broadcast(EShopPage::Coin);
}

void UW_ShopNavigationBar::ShowItemPage()
{
	OnShopPageRequested.Broadcast(EShopPage::Item);
}

void UW_ShopNavigationBar::ShowCardPage()
{
	OnShopPageRequested.Broadcast(EShopPage::Card);
}

void UW_ShopNavigationBar::ShowUnlockWeaponPage()
{
	OnShopPageRequested.Broadcast(EShopPage::UnlockWeapon);
}

void UW_ShopNavigationBar::ShowBossPage()
{
	OnShopPageRequested.Broadcast(EShopPage::Boss);
}

void UW_ShopNavigationBar::ShowMainPage()
{
	OnShopPageRequested.Broadcast(EShopPage::Main);
}