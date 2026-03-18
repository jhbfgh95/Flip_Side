// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	class UShopUnlockWeaponWSubsystem* UnlockWeaponSubsystem;
private:
	UPROPERTY(meta = (BindWidget))
	class UW_UnlockWeaponGrid* DealWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UW_UnlockWeaponGrid* TankWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UW_UnlockWeaponGrid* UtilWUnlockGrid;	

	class UW_UnlockWeaponGrid* CurrentUnlockGrid;
	
	UFUNCTION()
	void SetDealWUnlockGrid();
	UFUNCTION()
	void SetTankWUnlockGrid();
	UFUNCTION()
	void SetUtilWUnlockGrid();
	UFUNCTION()
	void SetUnlockWeaponGrid(EWeaponClass weaponClass);

	void HideAllUnlockGrid();
};
