// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinDataTypes.h"
#include "Styling/SlateTypes.h"
#include "BattleCoinSlotWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleCoinSlotWidgetClicked, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleCoinSlotWidgetHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleCoinSlotWidgetUnhovered, int32);

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UBattleCoinSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetSlotData(const FBattleCoinSlotViewData& InData);
	void ClearSlotData();
	void SetInfoSelected(bool bSelected);
	int32 GetSlotNumber() const { return SlotNumber; }

	FOnBattleCoinSlotWidgetClicked OnBattleCoinSlotClicked;
	FOnBattleCoinSlotWidgetHovered OnBattleCoinSlotHovered;
	FOnBattleCoinSlotWidgetUnhovered OnBattleCoinSlotUnhovered;

protected:
	// 실제 마우스 호버가 아니라, HUD에 현재 정보가 표시되고 있는 슬롯입니다.
	UPROPERTY(BlueprintReadOnly, Category = "Battle Coin Slot|Selection")
	bool bIsInfoSelected = false;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Slot|Selection")
	bool bUseHoveredStyleForInfoSelection = true;
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Coin Slot|Selection")
	void OnInfoSelectionChanged(bool bSelected);
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> CoinSlotButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> FrontCoinIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> FrontWeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BackCoinIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BackWeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> SlotNumberText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> HPText;

private:
	void RefreshInfoSelectionStyle();
	UPROPERTY(Transient)
	FButtonStyle UnselectedButtonStyle;
	bool bHasUnselectedButtonStyle = false;
	UFUNCTION()
	void HandleCoinButtonClicked();

	UFUNCTION()
	void HandleCoinButtonHovered();

	UFUNCTION()
	void HandleCoinButtonUnhovered();

	void UpdateWeaponIconMaterial(
		class UImage* WeaponIconImage,
		class UTexture2D* WeaponIconTexture,
		TObjectPtr<class UMaterialInstanceDynamic>& DynamicMaterial,
		const FLinearColor& WeaponColor
	);

	int32 SlotNumber = INDEX_NONE;
	int32 AvailableCoinCount = 0;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> FrontWeaponIconMaterialInstance;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> BackWeaponIconMaterialInstance;
};
