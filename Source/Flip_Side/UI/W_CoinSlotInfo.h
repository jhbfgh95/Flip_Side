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

	/** 스탯 아이콘 텍스처는 DB 연결 전까지 Widget BP에서 임시 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Coin Slot Info|Dummy Presentation")
	TObjectPtr<class UTexture2D> AttackPowerIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Coin Slot Info|Dummy Presentation")
	TObjectPtr<class UTexture2D> WeaponPowerIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Coin Slot Info|Dummy Presentation")
	TObjectPtr<class UTexture2D> CountIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Coin Slot Info|Dummy Presentation")
	FLinearColor AttackPowerColor = FLinearColor(1.0f, 0.32f, 0.28f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Coin Slot Info|Dummy Presentation")
	FLinearColor WeaponPowerColor = FLinearColor(0.25f, 0.65f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Coin Slot Info|Dummy Presentation")
	FLinearColor CountColor = FLinearColor(1.0f, 0.76f, 0.2f, 1.0f);

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredFrontWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredBackWeaponIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredFrontWeaponName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredBackWeaponName;

	/** 설명 토큰과 별개로 항상 표시되는 앞면 기본 스탯입니다. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrontAttackPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrontWeaponPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrontCountText;

	/** 설명 토큰과 별개로 항상 표시되는 뒷면 기본 스탯입니다. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackAttackPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackWeaponPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackCountText;
	
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
	void SetDetailedStatsVisible(bool bVisible);

private:
	void ConfigureDescriptionRichText(class URichTextBlock* RichTextBlock);
	void RefreshWeaponDescriptions();
	FText FormatWeaponDescription(const FWeaponDescriptionDisplayData& DisplayData) const;
	void SetWeaponInfo(
		bool bFrontFace,
		class UTexture2D* Icon,
		const FText& WeaponName,
		const FWeaponDescriptionDisplayData& DisplayData,
		const FLinearColor& WeaponColor
	);

	FBattleCoinSlotViewData CachedCoinSlotInfo;
	bool bHasCachedCoinSlotInfo = false;
	bool bShowDetailedStats = false;
};
