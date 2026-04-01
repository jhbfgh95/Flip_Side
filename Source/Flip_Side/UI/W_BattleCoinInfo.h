// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

public:
	void UpdateBattleCoinInfo(
		class UTexture2D* Icon, 
		const FText& WeaponName, 
		const FText& RawDescription, 
		int32 DefaultBP, int32 ModifiedBP, 
		int32 DefaultAP, int32 ModifiedAP,
		FLinearColor WeaponColor
		/*int32 DefaultRange, int32 ModifiedRange */
	);
};
