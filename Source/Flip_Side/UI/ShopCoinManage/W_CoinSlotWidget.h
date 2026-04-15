// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CoinSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinSlotWidget : public UUserWidget
{
	GENERATED_BODY()

virtual void NativeConstruct() override;
virtual void NativeDestruct() override;
private:
	class UShopCoinWSubsystem* CoinSubsystem;
	class UDataManagerSubsystem* DataSubsystem;
private:
	UPROPERTY(meta = (BindWidget))
	class UImage* LockImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* FrontWeaponImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackWeaponImage;
	
	UPROPERTY(meta = (BindWidget))
	class UW_CoinCountButton* IncreaseButton;
	
	UPROPERTY(meta = (BindWidget))
	class UW_CoinCountButton* DecreaseButton;

private:
	UPROPERTY()
	class UMaterialInstanceDynamic* FrontDynamicMaterial;
	UPROPERTY()
	class UMaterialInstanceDynamic* BackDynamicMaterial;
	UPROPERTY(EditAnywhere,meta =(AllowPrivateAccess))
	int32 SlotIndex;


	UFUNCTION()
	void PressSlotButton();
public:
	UFUNCTION()
	void SetWeaponTexture(int32 WeaponID);
	UFUNCTION()
	void UnlockSlot();
};
