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
class UW_ShopSlotCoinDes;
class UW_ShopCoinWarning;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveCurrentCoinWeaponRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveAllCoinWeaponsRequested);
UCLASS()
class FLIP_SIDE_API UW_ShopCoinWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RemoveCurrentWeaponButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RemoveAllWeaponsButton;

	UFUNCTION()
	void HandleRemoveCurrentWeaponClicked();

	UFUNCTION()
	void HandleRemoveAllWeaponsClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopWeaponSlotContainer> ShopWeaponSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinSlotContainer> ShopCoinSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_BuyCoinSlotContainer> BuyCoinSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopSlotCoinDes> WeaponDes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCoinWarning> ShopCoinWarning;
	
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnRemoveCurrentCoinWeaponRequested OnRemoveCurrentWeaponRequested;

	UPROPERTY(BlueprintAssignable)
	FOnRemoveAllCoinWeaponsRequested OnRemoveAllWeaponsRequested;

	UW_ShopCoinWarning* GetShopCoinWarning() const { return ShopCoinWarning.Get(); }

	UFUNCTION()
	UW_ShopWeaponSlotContainer* GetShopWeaponSlotContainer() const;

	UFUNCTION()
	UW_ShopCoinSlotContainer* GetShopCoinSlotContainer() const;

	UFUNCTION()
	UW_BuyCoinSlotContainer* GetBuyCoinSlotContainer() const;

	UW_ShopSlotCoinDes* GetWeaponDescription() const;
};
