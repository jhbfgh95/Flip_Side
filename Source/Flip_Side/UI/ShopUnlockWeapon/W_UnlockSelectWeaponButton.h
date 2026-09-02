// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockSelectWeaponButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedUnlockSelectedWeapon);

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockSelectWeaponButton : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* UnlockButton;

public:
	FOnClickedUnlockSelectedWeapon OnClickedUnlockSelectedWeapon;

	void ShowButton();
	void HideButton();

	UFUNCTION()
	void UnlockSelectWeapon();
};
