// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponButton : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	virtual void NativeOnInitialized() override;

private:
	class UUnlockGISubsystem* UnlockSubSystem; 
	class UShopWeaponDataWSubsystem* WeaponDataSubSystem;


private:

	UPROPERTY(meta = (BindWidget))
	class UImage* LockImage;
	
	
	UPROPERTY(meta = (BindWidget))
	class UButton* UnlockButton;

	const FFaceData* WeaponData;

public:
	UPROPERTY(EditAnywhere)
	EWeaponClass WeaponClass;

	UPROPERTY(EditAnywhere)
	int32 Index;

	UFUNCTION()
	void UnlockWeapon();

	void InitButton(EWeaponClass WeaponClassValue, int32 IndexValue);

	void SetButtonLock();
};
