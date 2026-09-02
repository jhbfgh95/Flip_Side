// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/W_ShopNavigationBar.h"
#include "W_ShopWidgetContainer.generated.h"

/**
 * 
 */
class UW_ShopItemWidget;
class UW_ShopCardMainWidget;
class UW_ShopCoinWidget;
class UW_UnlockWeaponWidget;
class UW_ShopCheckStartGame;
class UWidgetSwitcher;

UCLASS()
class FLIP_SIDE_API UW_ShopWidgetContainer : public UUserWidget
{
	GENERATED_BODY()


	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopNavigationBar> ShopNavigationBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ShopContentSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> ShopMainWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopItemWidget> ShopItemWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_UnlockWeaponWidget> ShopUnlockWeaponWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinWidget> ShopCoinWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCardMainWidget> ShopCardWidget;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopBossWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCheckStartGame> ShopCheckStartGameWidget;

private:
	UFUNCTION()
	void HandleShopPageRequested(EShopPage Page);

	UFUNCTION()
	void HandleStartGameConfirmed();


public:
	UPROPERTY(BlueprintAssignable)
	FOnShopPageRequested OnShopPageRequested;
	
	void HideShopContent();
	void ShowShopPage(EShopPage Page);

	UW_ShopItemWidget* GetShopItemWidget();
	
	UW_ShopCardMainWidget* GetShopCardWidget();

	UW_ShopCoinWidget* GetShopCoinWidget();

	UW_UnlockWeaponWidget* GetShopUnlockWeaponWidget();

	UW_ShopCheckStartGame* GetShopCheckStartWidget();
};
