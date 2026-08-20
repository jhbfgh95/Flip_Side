#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopNavigationBar.generated.h"

class UButton;

UENUM(BlueprintType)
enum class EShopPage : uint8
{
	Coin,
	Item,
	Card,
	UnlockWeapon,
	Boss
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopPageRequested, EShopPage, Page);

UCLASS()
class FLIP_SIDE_API UW_ShopNavigationBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

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

public:
	FOnShopPageRequested OnShopPageRequested;

private:
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
};
