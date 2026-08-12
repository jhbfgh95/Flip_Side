// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponSlot.generated.h"

/**
 * 
 */
class UImage;
class UButton;
class UTextBlock;
class UShopUnlockWeaponWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
protected:
	UPROPERTY()
	TObjectPtr<UShopUnlockWeaponWSubsystem> UnlockWeaponSubsystem;
	
protected:
	UPROPERTY()
	TObjectPtr<UImage> WeaponImage;
	
	UPROPERTY()
	TObjectPtr<UButton> SelectButton;

	UPROPERTY()
	TObjectPtr<UTextBlock> WeaponName;

protected:
	FFaceData UnlockWeaponData;

public:
	void InitWidget(FFaceData InitWeaponData, UShopUnlockWeaponWSubsystem* InitSubsystem);

protected:
	UFUNCTION()
	void ClickSlot();
	UFUNCTION()
	void HoverSlot();
	UFUNCTION()
	void UnhoverSlot();
};
