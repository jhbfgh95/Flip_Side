// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemPurchasePopup.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPurchaseItemRequested, int32, ItemID, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPurchasePopupCancelled);

UCLASS()
class FLIP_SIDE_API UW_ShopItemPurchasePopup : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalPriceText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CountPlusButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CountMinusButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PurchaseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;

	FItemData CurrentItemData;
	int32 CurrentCount = 1;

public:
	FOnPurchaseItemRequested OnPurchaseItemRequested;
	FOnPurchasePopupCancelled OnCancelled;

	void Open(const FItemData& InItemData);
	void Close();

private:
	UFUNCTION() void ClickPlus();
	UFUNCTION() void ClickMinus();
	UFUNCTION() void ClickConfirm();
	UFUNCTION() void ClickCancel();
};
