// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_WarningSameWeapon.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_WarningSameWeapon : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	class UCoinCreateWSubsystem* CoinCreateSubsystem;


	UFUNCTION()
	void OpenWarning();
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* WarningTextAnim;
	
};
