// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_Status.h"
#include "Blueprint/UserWidget.h"
#include "W_BattleCoinInfo.generated.h"

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
		FLinearColor WeaponColor,
		const TArray<FBuffInfo>& ActiveBuffs
		/*int32 DefaultRange, int32 ModifiedRange */
	);
};
