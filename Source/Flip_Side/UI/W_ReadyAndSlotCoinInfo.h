// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ReadyAndSlotCoinInfo.generated.h"

UCLASS()
class FLIP_SIDE_API UW_ReadyAndSlotCoinInfo : public UUserWidget
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

	UPROPERTY()
	class UMaterialInstanceDynamic* FrontDynamicMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* BackDynamicMaterial;

public:
	//FaceFlag true면 앞 false면 뒤
	void SetReadyCoinInfo(
		bool FaceFlag,
		class UTexture2D* Icon,
		const FText& WeaponName, 
		const FText& RawDescription,
		int32 DefaultBP, 
		int32 DefaultAP,
		FLinearColor typeColor
	);
};
