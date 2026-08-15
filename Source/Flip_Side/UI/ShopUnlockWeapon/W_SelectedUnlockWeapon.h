// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_SelectedUnlockWeapon.generated.h"

/**
 * 
 */
class UShopUnlockWeaponWSubsystem;
class UImage;
UCLASS()
class FLIP_SIDE_API UW_SelectedUnlockWeapon : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<UShopUnlockWeaponWSubsystem> UnlockWeaponSubsystem;
	
protected:
	UPROPERTY()
	TObjectPtr<UImage> SelectedWeaponImage;

public:
	UFUNCTION()
	void SetSelectedImage(FFaceData SelectedWeapon);

};
