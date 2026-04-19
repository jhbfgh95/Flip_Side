// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockWeaponGridContainer.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponGridContainer : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	class UUserWidget* DealWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UUserWidget* TankWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UUserWidget* UtilWUnlockGrid;	
	
	UPROPERTY(meta = (BindWidget))
	class UButton* DealClassButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* TankClassButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UtilClassButton;
	
	UFUNCTION()
	void SetDealWUnlockGrid();
	UFUNCTION()
	void SetTankWUnlockGrid();
	UFUNCTION()
	void SetUtilWUnlockGrid();
};
