// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponSlot.generated.h"

/**
 * 
 */
class UImage;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedUnlockWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredUnlockWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredUnlockWeaponSlot);

UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;

protected:
	FFaceData UnlockWeaponData;

public:
	FOnClickedUnlockWeaponSlot OnClickedUnlockWeaponSlot;
	FOnHoveredUnlockWeaponSlot OnHoveredUnlockWeaponSlot;
	FOnUnhoveredUnlockWeaponSlot OnUnhoveredUnlockWeaponSlot;

	void InitWidget(const FFaceData& InitWeaponData);
	int32 GetWeaponID() const { return UnlockWeaponData.WeaponID; }

protected:
	UFUNCTION()
	void ClickSlot();
	UFUNCTION()
	void HoverSlot();
	UFUNCTION()
	void UnhoverSlot();
};
