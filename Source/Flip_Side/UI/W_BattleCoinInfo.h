// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_Status.h"
#include "Blueprint/UserWidget.h"
#include "W_BattleCoinInfo.generated.h"

struct FBattleCoinAlternateFaceInfo
{
	bool bIsValid = false;
	class UTexture2D* Icon = nullptr;
	FText WeaponName;
	FText RawDescription;
	int32 DefaultBP = 0;
	int32 ModifiedBP = 0;
	int32 DefaultAP = 0;
	int32 ModifiedAP = 0;
};

UCLASS()
class FLIP_SIDE_API UW_BattleCoinInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredWeaponIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredWeaponName;
	
	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* HoveredWeaponDes;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinCurrentHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinMaxHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinSlotIndexText;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredBuffIcon1;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredBuffIcon2;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredBuffIcon3;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredBuffIcon4;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredBuffIcon5;

	UPROPERTY()
	TArray<class UImage*> BuffIconImages;

	void CacheBuffIconImages();
	void SetBuffIconImage(class UImage* BuffImage, class UTexture2D* Icon);
	void UpdateBuffIcons(const TArray<FBuffInfo>& ActiveBuffs);

public:
	void UpdateBattleCoinInfo(
		class UTexture2D* Icon,
		const FText& WeaponName,
		const FText& RawDescription,
		int32 DefaultBP, int32 ModifiedBP,
		int32 DefaultAP, int32 ModifiedAP,
		int32 CurrentHP, int32 MaxHP,
		FLinearColor WeaponColor,
		const TArray<FBuffInfo>& ActiveBuffs,
		int32 AbsorbedBP,
		int32 SlotIndex,
		const FBattleCoinAlternateFaceInfo& AlternateFaceInfo
		/*int32 DefaultRange, int32 ModifiedRange */
	);
};
