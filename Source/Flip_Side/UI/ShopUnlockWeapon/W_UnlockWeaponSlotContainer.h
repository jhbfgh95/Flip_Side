// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponSlotContainer.generated.h"

/**
 * 
 */
class UVerticalBox;
class UW_UnlockWeaponSlot;
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
	TSubclassOf<UW_UnlockWeaponSlot> UnlockWeaponSlot;

public:
	void InitWidget(const TArray<FFaceData>& InWeaponDataArray);
	void AddWeaponSlot(const FFaceData& InWeaponData);
	bool RemoveWeaponSlot(int32 WeaponID);
	const TArray<TObjectPtr<UW_UnlockWeaponSlot>>& GetWeaponSlots() const { return UnlockWeaponSlots; }

};
