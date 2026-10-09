// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemSlotContainer.generated.h"

/**
 * 
 */
class UW_ShopItemSlot;
class UVerticalBox;
class UBorder;
UCLASS()
class FLIP_SIDE_API UW_ShopItemSlotContainer : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopItemSlot>> ShopItemSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ShopItemBorder;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> SellItemSlot;

public:
	UBorder* GetShopItemBorder() const { return ShopItemBorder.Get(); }
	void InitWidget(TArray<FItemData> InItemDataArray);

	void SetItemSlotInputEnabled(bool bInEnabled);

	TArray<TObjectPtr<UW_ShopItemSlot>> GetShopItemSlots();
};
