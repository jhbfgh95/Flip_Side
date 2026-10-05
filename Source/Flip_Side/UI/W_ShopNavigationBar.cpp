#include "UI/W_ShopNavigationBar.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"

void UW_ShopNavigationBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	NavigationToggleButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ToggleNavigationBar);
	MainButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowMainPage);
	CoinButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowCoinPage);
	ItemButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowItemPage);
	CardButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowCardPage);
	UnlockWeaponButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowUnlockWeaponPage);
	BossButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowBossPage);
	StartGameButton->OnClicked.AddDynamic(this, &UW_ShopNavigationBar::ShowStartGamePage);

	SelectedButton = MainButton;
}

void UW_ShopNavigationBar::ToggleNavigationBar()
{
	bNavigationBarOpen = !bNavigationBarOpen;
	OnNavigationToggleClicked.Broadcast();
	PlayAnimation(
		SlideNavigationAnim,
		0.f,
		1,
		bNavigationBarOpen
		? EUMGSequencePlayMode::Forward
		: EUMGSequencePlayMode::Reverse);
}

void UW_ShopNavigationBar::SetSelectedPageButton(EShopPage Page)
{
	FButtonStyle ButtonStyle = SelectedButton->GetStyle();
	FLinearColor NormalTint = ButtonStyle.Normal.TintColor.GetSpecifiedColor();
	NormalTint.A = 0;
	ButtonStyle.Normal.TintColor = FSlateColor(NormalTint);
	SelectedButton->SetStyle(ButtonStyle);
	

	if (SlideNavigationAnim)
	switch (Page)
	{

	case EShopPage::Main:         SelectedButton = MainButton; break;
	case EShopPage::Coin:         SelectedButton = CoinButton; break;
	case EShopPage::Item:         SelectedButton = ItemButton; break;
	case EShopPage::Card:         SelectedButton = CardButton; break;
	case EShopPage::UnlockWeapon: SelectedButton = UnlockWeaponButton; break;
	case EShopPage::Boss:         SelectedButton = BossButton; break;
	case EShopPage::GameStart:    SelectedButton = StartGameButton; break;
	default: return;
	}

	if (!IsValid(SelectedButton))
		return;

	NormalTint.A = 0.3f;
	ButtonStyle.Normal.TintColor = FSlateColor(NormalTint);
	SelectedButton->SetStyle(ButtonStyle);
}

void UW_ShopNavigationBar::SetNavigationButtonsLocked(bool bLockAll, EShopPage AllowedPage)
{
	if(bLockAll&& AllowedPage==EShopPage::None)
	{
		bNavigationBarOpen = false;
		PlayAnimation(
			SlideNavigationAnim,
			0.f,
			1, EUMGSequencePlayMode::Reverse);
	}

	const bool bEnableAllButtons = !bLockAll && AllowedPage == EShopPage::None;
	const bool bEnableMainButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::Main);
	const bool bEnableCoinButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::Coin);
	const bool bEnableItemButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::Item);
	const bool bEnableCardButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::Card);
	const bool bEnableUnlockWeaponButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::UnlockWeapon);
	const bool bEnableBossButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::Boss);
	const bool bEnableStartGameButton = !bLockAll && (bEnableAllButtons || AllowedPage == EShopPage::GameStart);

	if (IsValid(NavigationToggleButton)) 
		NavigationToggleButton->SetIsEnabled(!bLockAll);
	if (IsValid(MainButton)) MainButton->SetIsEnabled(bEnableMainButton);
	if (IsValid(CoinButton)) CoinButton->SetIsEnabled(bEnableCoinButton);
	if (IsValid(ItemButton)) ItemButton->SetIsEnabled(bEnableItemButton);
	if (IsValid(CardButton)) CardButton->SetIsEnabled(bEnableCardButton);
	if (IsValid(UnlockWeaponButton)) UnlockWeaponButton->SetIsEnabled(bEnableUnlockWeaponButton);
	if (IsValid(BossButton)) BossButton->SetIsEnabled(bEnableBossButton);
	if (IsValid(StartGameButton)) StartGameButton->SetIsEnabled(bEnableStartGameButton);
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
	
void UW_ShopNavigationBar::ShowStartGamePage()
{
	OnShopPageRequested.Broadcast(EShopPage::GameStart);
}
