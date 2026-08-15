// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockWeaponSlotContainer.generated.h"

/**
 * 
 */
class UVerticalBox;
class UW_UnlockWeaponSlot;
class UUnlockGISubsystem;
class UShopUnlockWeaponWSubsystem;
class UDataManagerSubsystem;
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponSlotContainer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:

	UPROPERTY()
	TArray<TObjectPtr<UW_UnlockWeaponSlot>> UnlockWeaponSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> UnlockWeaponSlotBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> UnlockWeaponSlot;

protected:
	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;

	UPROPERTY()
	TObjectPtr<UShopUnlockWeaponWSubsystem> UnlockWeaponSubsystem;

	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManagerSubsystem;

};
