// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopModeChangeWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopModeChangeWidget : public UUserWidget
{
	GENERATED_BODY()



protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* WidgetOpenButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CoinCreateModeButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CardSelectModeButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ShopItemModeButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CheckBossModeButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ShopMainModeButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* UnlockWeaponButton;

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

};
