// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCoinSlotBuyButton.generated.h"

/**
 * 
 */
class UButton;
class UShopCoinWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinSlotBuyButton : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeOnInitialized() override;
protected:
	UPROPERTY()
	TObjectPtr<UButton> BuySlotButton;

protected:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> CoinSubsystem;

protected:
	UPROPERTY(EditAnywhere)
	int32 ButSlotLevel= 1;
protected:
	UFUNCTION()
	void ClickBuySlotButton();
};
