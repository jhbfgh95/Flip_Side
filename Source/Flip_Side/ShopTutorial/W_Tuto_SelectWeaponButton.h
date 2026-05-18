// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinCreate/W_SelectWeaponButton.h"
#include "W_Tuto_SelectWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_SelectWeaponButton : public UW_SelectWeaponButton
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 InitID = -1;

	class UShopTutorialWSubsystem* TutorialSubsystem;

	UFUNCTION()
	void ExcuteTutorial();
};
