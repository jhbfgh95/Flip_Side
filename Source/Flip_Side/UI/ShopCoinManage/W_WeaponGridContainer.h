// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_WeaponGridContainer.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_WeaponGridContainer : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	class UShopCoinWSubsystem* CoinSubsystem;


private:

	UPROPERTY(meta = (BindWidget))
	class UButton* DealGridButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* TankGridButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* UtilGridButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* LockImage;

	UPROPERTY(meta = (BindWidget))
	class UW_WeaponSelectGrid* DealGrid;
	UPROPERTY(meta = (BindWidget))
	class UW_WeaponSelectGrid* TankGrid;
	UPROPERTY(meta = (BindWidget))
	class UW_WeaponSelectGrid* UtilGrid;


private:
	UFUNCTION()
	void SetUnlockImageSlotChange();
	UFUNCTION()
	void SetUnlockImage();

	UFUNCTION()
	void SetDealGrid();
	UFUNCTION()
	void SetTankGrid();
	UFUNCTION()
	void SetUtilGrid();

};
