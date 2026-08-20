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
class UW_ShopCoinSlot;
class UScrollBox;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinSlotContainer : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> CoinSlotScrollBox;

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

public:
	void InitWidget();
	
	void AddCoinSlot(int32 AddSlotIndex, int32 Hp);
	
	void RemoveCoinSlot(int32 RemoveSlotIndex);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class UUserWidget> CoinSlotWidget;

	UPROPERTY()
	TArray<TObjectPtr<UW_ShopCoinSlot>> CoinSlots;

private:
	void UpdateSlotText(bool IsIncrease);


public:
	const TArray<TObjectPtr<UW_ShopCoinSlot>>& GetCoinSlots() const { return CoinSlots; }

};	
