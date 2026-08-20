// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_ShopWeaponSlotContainer.generated.h"

/**
 * 
 */
class UW_ShopCoinWeaponSlot;
class UVerticalBox;
class UShopCoinWSubsystem;
class UUnlockGISubsystem;
class UDataManagerSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopWeaponSlotContainer : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	

protected:
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopCoinWeaponSlot>> WeaponSlotArray;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UW_ShopCoinWeaponSlot> WepoanSlotWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

public:
	void InitWidget(TArray<FFaceData> InWeaponData);

	UW_ShopCoinWeaponSlot* AddWeaponSlot(const FFaceData& InWeaponData);
	
	const TArray<TObjectPtr<UW_ShopCoinWeaponSlot>>& GetWeaponSlots() const { return WeaponSlotArray; }
};
