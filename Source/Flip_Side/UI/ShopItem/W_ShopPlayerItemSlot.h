// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopPlayerItemSlot.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerItemSlot : public UUserWidget
{
		GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> ItemSellButton;
	
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

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemSellCountTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> EmptySlotImage;

protected:
	UPROPERTY()
	class UShopItemWSubsystem* ItemSubsystem;
	UPROPERTY()
	TObjectPtr<class UMoneyGISubsystem> MoneySubsystem;

protected:
	FItemData WidgetItemData;
	int32 CurrentItemCount = 0;
	UPROPERTY(EditAnywhere)
	int32 PlayerInvenIndex = -1;
public:
	void InitItemWidget(int32 InitIndex);

public:
	UFUNCTION()
	void ClickItemSellButton();
	UFUNCTION()
	void ClickItemCountPlusButton();
	UFUNCTION()
	void ClickItemCountMinusButton();

public:
	UFUNCTION()
	void SetItemWidget(int32 BuyItemIndex);

	UFUNCTION()
	void DeleteItemWidget();

protected:
	virtual FReply NativeOnMouseButtonDown(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnDragDetected(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent,
        UDragDropOperation*& OutOperation) override;

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;

};
