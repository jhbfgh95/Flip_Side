// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCoinWidget.generated.h"

/**
 * 
 */
class UW_ShopWeaponSlotContainer;
class UW_ShopCoinSlotContainer;
class UW_ShopCoinSlotBuyButton;
class UW_ShopSelectCoin;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopWeaponSlotContainer> ShopWeaponSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinSlotContainer> ShopCoinSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinSlotBuyButton> L1_ShopCoinSlotBuyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinSlotBuyButton> L2_ShopCoinSlotBuyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinSlotBuyButton> L3_ShopCoinSlotBuyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopSelectCoin> ShopSelectCoin;
	
	
public:
	UFUNCTION()
	UW_ShopWeaponSlotContainer* GetShopWeaponSlotContainer() const;

	UFUNCTION()
	UW_ShopCoinSlotContainer* GetShopCoinSlotContainer() const;

	UFUNCTION()
	UW_ShopCoinSlotBuyButton* GetShopCoinSlotBuyButton(int32 GetButtonLevel);

	UW_ShopSelectCoin* GetShopSelectCoin();
};
