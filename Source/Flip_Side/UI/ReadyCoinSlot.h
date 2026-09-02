// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinDataTypes.h"
#include "ReadyCoinSlot.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadyCoinSlotWidgetClicked, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadyCoinSlotWidgetHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadyCoinSlotWidgetUnhovered, int32);

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UReadyCoinSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetReadyCoinData(const FBattleReadyCoinViewData& InData);
	void ClearReadyCoinData();

	FOnReadyCoinSlotWidgetClicked OnReadyCoinSlotClicked;
	FOnReadyCoinSlotWidgetHovered OnReadyCoinSlotHovered;
	FOnReadyCoinSlotWidgetUnhovered OnReadyCoinSlotUnhovered;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> ReadyCoinButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> FrontWeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BackWeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> FrontCoinIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BackCoinIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ReadySlotNumberText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> HPText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CanCancleText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> CanCancleIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Ready Coin|Cancel State")
	TObjectPtr<class UTexture2D> CanCancelIconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Ready Coin|Cancel State")
	TObjectPtr<class UTexture2D> CannotCancelIconTexture;

private:
	UFUNCTION()
	void HandleReadyCoinClicked();

	UFUNCTION()
	void HandleReadyCoinHovered();

	UFUNCTION()
	void HandleReadyCoinUnhovered();

	void UpdateCancelStateVisual();
	void UpdateWeaponIconMaterial(
		class UImage* WeaponIconImage,
		class UTexture2D* WeaponIconTexture,
		TObjectPtr<class UMaterialInstanceDynamic>& DynamicMaterial,
		const FLinearColor& WeaponColor
	);

	int32 CoinInstanceID = INDEX_NONE;
	bool bCanCancel = false;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> CancelStateIconMaterialInstance;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> FrontWeaponIconMaterialInstance;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> BackWeaponIconMaterialInstance;
};
