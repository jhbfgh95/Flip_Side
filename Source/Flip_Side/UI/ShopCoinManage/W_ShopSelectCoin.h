// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/CoinDataTypes.h"
#include "W_ShopSelectCoin.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeShopSelectedCoinSide);

/**
 * 
 */
class UButton;
class UImage;
class UWidgetAnimation;
class UMaterialInstanceDynamic;
class UTexture2D;
UCLASS()
class FLIP_SIDE_API UW_ShopSelectCoin : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> ChangeCoinSideButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> FrontWeaponImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> BackWeaponImage;
protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ChangeCoinSideAnim;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> DefaultsTexture;
public:

	FOnChangeShopSelectedCoinSide OnChangeShopSelectedCoinSide;

public:
	void SetFrontWeapon(FFaceData FrontWeaponFaceData);
	void SetBackWeapon(FFaceData BackWeaponFaceData);
	
	void ResetCoin();
	void ChangeCoinSide(bool IsFront);

protected:
	UFUNCTION()
	void ClickChangeSideButton();
};
