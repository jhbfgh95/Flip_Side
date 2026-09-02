// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_ShopCoinWeaponSlot.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UButton;
class UBorder;
class UW_ShopCoinWeaponSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedShopCoinWeaponSlot, UW_ShopCoinWeaponSlot*, WeaponSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredShopCoinWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredShopCoinWeaponSlot);

UCLASS()
class FLIP_SIDE_API UW_ShopCoinWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

protected:
	FFaceData WeaponData;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> WeaponIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> WeaponSelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> HoverBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SelectBorder;

private:
	bool IsSelected = false;
public:

	UPROPERTY(BlueprintAssignable, Category = "Shop Coin Weapon Slot|Event")
	FOnClickedShopCoinWeaponSlot OnClickedShopCoinWeaponSlot;

	UPROPERTY(BlueprintAssignable, Category = "Shop Coin Weapon Slot|Event")
	FOnHoveredShopCoinWeaponSlot OnHoveredShopCoinWeaponSlot;

	UPROPERTY(BlueprintAssignable, Category = "Shop Coin Weapon Slot|Event")
	FOnUnhoveredShopCoinWeaponSlot OnUnhoveredShopCoinWeaponSlot;

	UFUNCTION()
	void InitWidget(FFaceData FaceData);

	UFUNCTION()
	void ClickWidget();
	
	UFUNCTION()
	void HoverWidget();

	UFUNCTION()
	void UnhoverWidget();

	void SelectSlot();

	void UnSelectSlot();
	
	int32 GetWeaponSlotID();

};
