// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopWidgetContainer.generated.h"

/**
 * 
 */
class UW_ShopItemWidget;
class UW_ShopCardMainWidget;
class UW_ShopCoinWidget;
UCLASS()
class FLIP_SIDE_API UW_ShopWidgetContainer : public UUserWidget
{
	GENERATED_BODY()


	virtual void NativeConstruct() override;

private:
	class AGameMode_Shop* ShopGameMode;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopItemWidget> ShopItemWidget;
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopUnlockWeaponWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinWidget> ShopCoinWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCardMainWidget> ShopCardWidget;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* ShopBossWidget;

private:
	UFUNCTION()
	void SetShopItemWidget();
	UFUNCTION()
	void SetShopUnlockWeaponWidget();
	UFUNCTION()
	void SetShopCoinWidget();
	UFUNCTION()
	void SetShopCardWidget();
	UFUNCTION()
	void SetShopBossWidget();
	
private:
	void HideAllWidget();
	void SetWidget(UUserWidget* HideWidget);

public:
	UW_ShopItemWidget* GetShopItemWidget();
	
	UW_ShopCardMainWidget* GetShopCardWidget();

	UW_ShopCoinWidget* GetShopCoinWidget();
};
