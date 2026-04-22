// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CoinSlotContainer.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinSlotContainer : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

	UPROPERTY(meta  =(BindWidget))
	class UTextBlock* TotalCoinText;

	UFUNCTION()
	void SetTotalCoinText(int32 ChangedSlotIndex, int32 Count);
};
