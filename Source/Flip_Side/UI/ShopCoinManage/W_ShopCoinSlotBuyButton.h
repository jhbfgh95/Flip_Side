// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCoinSlotBuyButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedShopCoinSlotBuyButton, int32, BuySlotLevel);

/**
 * 
 */
class UButton;
class UTextBlock;
class UDataManagerSubsystem;
class UShopCoinWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinSlotBuyButton : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeOnInitialized() override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BuySlotButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BuySlotPriceTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BuySlotHpTextBlock;
protected:
	UPROPERTY(EditAnywhere)
	int32 BuySlotLevel= 1;

	int32 SlotPrice = 0;
	int32 BuySlotHp = 0;
public:
	UPROPERTY(BlueprintAssignable)
	FOnClickedShopCoinSlotBuyButton OnClickedShopCoinSlotBuyButton;

	UFUNCTION(BlueprintCallable)
	void InitWidget(int32 InBuySlotLevel, int32 InSlotPrice, int32 InBuySlotHp);
protected:
	UFUNCTION()
	void ClickBuySlotButton();

};
