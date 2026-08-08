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
class UShopCoinWSubsystem;
class UDataManagerSubsystem;

UCLASS()
class FLIP_SIDE_API UW_ShopCoinWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
protected:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> CoinSubsystem;

protected:
	FFaceData WeaponData;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> WeaponIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> WeaponSelectButton;
public:
	UFUNCTION()
	void InitWidget(FFaceData FaceData, UShopCoinWSubsystem* InitCoinSubsystem);

	UFUNCTION()
	void ClickWidget();
	
	UFUNCTION()
	void HoverWidget();

	UFUNCTION()
	void UnhoverWidget();
	
};
