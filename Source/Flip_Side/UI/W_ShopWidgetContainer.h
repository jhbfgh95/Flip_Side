// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopWidgetContainer.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopWidgetContainer : public UUserWidget
{
	GENERATED_BODY()


	virtual void NativeConstruct() override;

private:
	class AGameMode_Shop* ShopGameMode;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopModeChangeWidet;
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopItemWidget;
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopUnlockWeaponWidget;
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopCoinWidget;
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopCardWidget;

private:
	UFUNCTION()
	void SetShopItemWidget();
	UFUNCTION()
	void SetShopUnlockWeaponWidget();
	UFUNCTION()
	void SetShopCoinWidget();
	UFUNCTION()
	void SetShopCardWidget();
	
private:
	void HideAllWidget();
	void SetWidget(UUserWidget* HideWidget);
};
