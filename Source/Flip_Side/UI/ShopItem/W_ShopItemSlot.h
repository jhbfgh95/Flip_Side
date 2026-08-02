// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemSlot.generated.h"

/**
 * 
 */
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

protected:
	TObjectPtr<class UShopItemWSubsystem> ItemSubsystem;
	TObjectPtr<class UMoneyGISubsystem> MoneySubsystem;

protected:
	FItemData WidgetItemData;
	int32 CurrentItemCount = 0;

public:
	void InitItemWidget(FItemData SetItemData);

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
