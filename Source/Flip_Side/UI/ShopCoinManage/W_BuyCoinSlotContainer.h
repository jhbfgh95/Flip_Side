// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDataTypes.h"
#include "W_BuyCoinSlotContainer.generated.h"

class UBorder;
class UButton;
class UDataManagerSubsystem;
class UVerticalBox;
class UW_ShopCoinSlotBuyButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuyCoinSlotRequested, int32, SlotLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuyCoinSlotPopupCancelled);

UCLASS()
class FLIP_SIDE_API UW_BuyCoinSlotContainer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OpenSlotBuyPopupButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBuyBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBuyButtonBox;

	UPROPERTY(EditAnywhere, Category = "Shop Coin")
	TSubclassOf<UW_ShopCoinSlotBuyButton> ShopCoinSlotBuyButtonClass;

	UPROPERTY()
	TArray<TObjectPtr<UW_ShopCoinSlotBuyButton>> SlotBuyButtons;

public:
	UPROPERTY(BlueprintAssignable)
	FOnBuyCoinSlotRequested OnBuyCoinSlotRequested;

	UPROPERTY(BlueprintAssignable)
	FOnBuyCoinSlotPopupCancelled OnBuyCoinSlotPopupCancelled;

private:
	bool IsPopupOpen = false;
public:
	// 전달된 슬롯 레벨 데이터로 구매 버튼을 만들고 가격/체력을 표시한다.
	void InitWidget(const TArray<FCoinTypeStructure>& InShopCoinSlotData,
		UDataManagerSubsystem* InDataManager);

	UFUNCTION(BlueprintCallable)
	void OpenSlotBuyPopupBorder();

	UFUNCTION(BlueprintCallable)
	void CloseSlotBuyPopupBorder();

private:
	UFUNCTION()
	void ClickOpenSlotBuyPopup();

	UFUNCTION()
	void ClickCancelButton();

	UFUNCTION()
	void ClickSlotBuyButton(int32 SlotLevel);
};
