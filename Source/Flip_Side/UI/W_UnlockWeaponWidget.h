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

private:
	UPROPERTY(meta = (BindWidget))
	class UW_UnlockWeaponGrid* DealWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UW_UnlockWeaponGrid* TankWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UW_UnlockWeaponGrid* UtilWUnlockGrid;	

	UPROPERTY(meta = (BindWidget))
	class UButton* DealWGridButton;	
	
	UPROPERTY(meta = (BindWidget))
	class UButton* TankWGridButton;	
	
	UPROPERTY(meta = (BindWidget))
	class UButton* UtilWGridButton;	

	UFUNCTION()
	void SetDealWUnlockGrid();
	UFUNCTION()
	void SetTankWUnlockGrid();
	UFUNCTION()
	void SetUtilWUnlockGrid();

	void HideAllUnlockGrid();
};
