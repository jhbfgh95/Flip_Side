// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemSlot.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuyItem, int32, ItemID, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredShopItemSlot, int32, ItemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAddBuyItemCount, UW_ShopItemSlot*, BuyItemSlot, int32, ItemID, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredShopItemSlot);

class UButton;
class UImage;
class UTextBlock;
UCLASS()
class FLIP_SIDE_API UW_ShopItemSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> ItemBuyButton;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> ItemCountPlusButton;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> ItemCountMinusButton;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCountTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemPriceTextBlock;

public:

	FOnBuyItem OnBuyItem;
	FOnHoveredShopItemSlot OnHoveredSlot;
	FOnUnhoveredShopItemSlot OnUnhoveredSlot;
	FOnAddBuyItemCount OnAddBuyItemCount;

protected:
	FItemData WidgetItemData;
	int32 CurrentItemCount = 0;

public:

	void InitItemWidget(FItemData SetItemData);
	void SetItemSlotImage(FItemData SetItemData);
	void AddBuyItemCount(int32 Count);

	void SetItemCount(int32 Count);
public:
	UFUNCTION()
	void ClickItemBuyButton();
	UFUNCTION()
	void ClickItemCountPlusButton();
	UFUNCTION()
	void ClickItemCountMinusButton();

	
protected:
    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;

};
