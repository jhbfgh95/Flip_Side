// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemSlot.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickShopItemSlot, UW_ShopItemSlot*, ClickedSlot, int32, ItemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredShopItemSlot, int32, ItemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredShopItemSlot);

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
	TObjectPtr<UImage> ItemImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemPriceTextBlock;

public:

	FOnClickShopItemSlot OnClickShopItemSlot;
	FOnHoveredShopItemSlot OnHoveredSlot;
	FOnUnhoveredShopItemSlot OnUnhoveredSlot;

protected:
	FItemData WidgetItemData;

	UPROPERTY(EditAnywhere, Category = "Shop Item|Hover")
	float HoverScale = 1.1f;

	FVector2D DefaultRenderScale = FVector2D(1.0f, 1.0f);

	void SetHoverScale(bool bHovered);

public:

	void InitItemWidget(FItemData SetItemData);
	void SetItemSlotImage(FItemData SetItemData);

	
protected:
    virtual FReply NativeOnMouseButtonDown(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;

};
