// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockSelectWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockSelectWeaponButton : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
	class UShopUnlockWeaponWSubsystem* UnlockWeaponSubsystem;
	UPROPERTY(meta = (BindWidget))
	class UButton* UnlockButton;

	UFUNCTION()
	void UnlockSelectWeapon();
};
