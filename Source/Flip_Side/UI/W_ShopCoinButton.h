// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCoinButton.generated.h"

/**
 * 
 */
class UButton;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinButton : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

	class UCoinCreateWSubsystem* CoinCreateSubsystem;

private:

	UPROPERTY(meta = (BindWidget))
	UButton* CoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CoinAddButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CoinReduceButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CoindistroyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotUpButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotDownButton;

	
	UPROPERTY(meta = (BindWidget))
	UButton* UnlockCoinSlotButton;

public:
	UPROPERTY(EditAnywhere)
	int32 CoinIndex;

	UFUNCTION()
	void CreateCoin();
	
	UFUNCTION()
	void IncreaseCoin();
	
	UFUNCTION()
	void DecreaseCoin();
	
	UFUNCTION()
	void ResetCoin();
	
	UFUNCTION()
	void ChangeSlotUp();

	UFUNCTION()
	void ChangeSlotDown();

	
	UFUNCTION()
	void UnlockCoinSlot();
};
