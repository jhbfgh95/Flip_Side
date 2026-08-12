// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinDataTypes.h"
#include "W_CoinSlotInfo.generated.h"

UCLASS()
class FLIP_SIDE_API UW_CoinSlotInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredFrontWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredBackWeaponIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredFrontWeaponName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredBackWeaponName;
	
	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* HoveredFrontWeaponDes;

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* HoveredBackWeaponDes;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinMaxHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinSlotNumberText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinCountText;

	UPROPERTY()
	class UMaterialInstanceDynamic* FrontDynamicMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* BackDynamicMaterial;

public:
	void SetCoinSlotInfo(const FBattleCoinSlotViewData& InData);

private:
	void SetWeaponInfo(
		bool bFrontFace,
		class UTexture2D* Icon,
		const FText& WeaponName,
		const FText& RawDescription,
		int32 DefaultBP,
		int32 DefaultAP,
		const FLinearColor& WeaponColor
	);
};
