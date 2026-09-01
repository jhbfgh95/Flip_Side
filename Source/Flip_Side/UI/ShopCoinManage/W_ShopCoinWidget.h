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
class UW_BuyCoinSlotContainer;
class UW_WeaponDescription;
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
	TObjectPtr<UW_BuyCoinSlotContainer> BuyCoinSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_WeaponDescription> WeaponDes;
	
	
public:
	UFUNCTION()
	UW_ShopWeaponSlotContainer* GetShopWeaponSlotContainer() const;

	UFUNCTION()
	UW_ShopCoinSlotContainer* GetShopCoinSlotContainer() const;

	UFUNCTION()
	UW_BuyCoinSlotContainer* GetBuyCoinSlotContainer() const;

	UW_WeaponDescription* GetWeaponDescription() const;
};
