// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCoinSlotContainer.generated.h"

/**
 * 
 */
class UTextBlock;
class UVerticalBox;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinSlotContainer : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

protected:
	
	UPROPERTY(meta  =(BindWidget))
	TObjectPtr<UVerticalBox> CoinSlotBox;
/*
	UPROPERTY(meta  =(BindWidget))
	class UTextBlock* TotalCoinText;

	UPROPERTY(meta  =(BindWidget))
	class UTextBlock* SlotInfoText;
*/
	UFUNCTION()
	void SetTotalCoinText(int32 ChangedSlotIndex, int32 Count);

protected:
	/*
	UPROPERTY(meta= (BindWidget))
	class UScrollBox* SlotScroll;

	UPROPERTY(meta= (BindWidget))
	class UButton* SlotAddButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* DecreaseSlotButton;

	UPROPERTY(meta = (BindWidget))
	class UContentWidget* SlotBuyDescrip;
*/
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class UUserWidget> CoinSlotWidget;

	TArray<class UW_ShopCoinSlot*> CoinSlots;

	int CoinSlotCount =0;

private:
	UFUNCTION()
	void AddCoinSlot();
	UFUNCTION()
	void DecreaseCoinSlot();
	UFUNCTION()
	void UpdateSlotText(bool IsIncrease);
	UFUNCTION()
	void CoinSlotChangeAdaptor(bool IsIncreaseSlot);

	void InitCoinSlot();

};	
