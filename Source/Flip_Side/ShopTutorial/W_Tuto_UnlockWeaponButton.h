// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponButton.h"
#include "W_Tuto_UnlockWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_UnlockWeaponButton : public UW_UnlockWeaponButton
{
	GENERATED_BODY()


	virtual void NativeOnInitialized() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 ExcuteOrder;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 InitID;

	UFUNCTION()
	void ExcuteEvent();
};
