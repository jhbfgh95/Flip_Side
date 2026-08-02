// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopWeaponSlotContainer.generated.h"

/**
 * 
 */
class UW_CoinWeaponSlot;
class UVerticalBox;
class UShopCoinWSubsystem;
class UUnlockGISubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopWeaponSlotContainer : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> ShopCoinSubsystem;
	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UW_CoinWeaponSlot>> WeaponSlotArray;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UW_CoinWeaponSlot> WepoanSlotWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

};
