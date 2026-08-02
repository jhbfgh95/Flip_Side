// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/CoinDataTypes.h"
#include "W_ShopSelectCoin.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UMaterialInstanceDynamic;
class UTexture2D;
class UDataManagerSubsystem;
class UShopCoinWSubsystem;
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
	TObjectPtr<UImage> FrontSideImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> BackSideImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> FrontWeaponImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> BackWeaponImage;
protected:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> CoinSubsystem;
	
	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;

protected:
	FFaceData FrontWeaponData;
	FFaceData BackWeaponData;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FrontDynamicMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BackDynamicMaterial;

	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> DefaultsTexture;

	FCoinTypeStructure CurrentCoinType;

public:
	void SetFrontWeapon(FFaceData FrontWeaponFaceData);
	void SetBackWeapon(FFaceData BackWeaponFaceData);
	void ChangeCoinSide();
	void SetCoin();

};
