// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopModeChangeWidget.generated.h"

/**
 * 
 */
class UTexture2D;
class UButton;
UCLASS()
class FLIP_SIDE_API UW_ShopModeChangeWidget : public UUserWidget
{
	GENERATED_BODY()



protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* WidgetOpenButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CoinCreateModeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* CardSelectModeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ShopItemModeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* CheckBossModeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ShopMainModeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* UnlockWeaponButton;

	class AGameMode_Shop* ShopGameMode;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlidePanelOpenAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlidePanelCloseAnim;

	bool IsSlidePanelOpen = false;

	UFUNCTION()
	void ClickCoinCreateMode();
	UFUNCTION()
	void ClickCardSelecrMode();
	UFUNCTION()
	void ClickShopItemMode();
	UFUNCTION()
	void ClickCheckBossMode();
	UFUNCTION()
	void OpenWidget();
	UFUNCTION()
	void ClickShopMainModeButton();
	UFUNCTION()
	void ClickUnlockWeaponMode();

	void SetButtonTexture(UTexture2D* SetButtonTexture);
};
