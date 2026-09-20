#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ShopPageTypes.h"
#include "W_ShopNavigationBar.generated.h"

class UButton;
class UWidgetAnimation;
class UW_ShopPageChangeButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopPageRequested, EShopPage, Page);

UCLASS()
class FLIP_SIDE_API UW_ShopNavigationBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainButton;

	// 바 하단에 배치해 접힌 상태에서도 보이는 토글(손잡이) 버튼입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NavigationToggleButton;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> SlideNavigationAnim;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CoinButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ItemButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CardButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UnlockWeaponButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BossButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

public:
	FOnShopPageRequested OnShopPageRequested;

private:
	bool bNavigationBarOpen = false;

	UFUNCTION()
	void ToggleNavigationBar();

	UFUNCTION()
	void ShowCoinPage();

	UFUNCTION()
	void ShowItemPage();

	UFUNCTION()
	void ShowCardPage();

	UFUNCTION()
	void ShowUnlockWeaponPage();

	UFUNCTION()
	void ShowBossPage();

	UFUNCTION()
	void ShowMainPage();

	UFUNCTION()
	void ShowStartGamePage();
};
