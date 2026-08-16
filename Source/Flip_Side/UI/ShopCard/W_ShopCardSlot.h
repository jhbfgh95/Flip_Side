// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ShopCard/W_ShopCardBase.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopCardSlot.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuyCard, UW_ShopCardSlot*, BuyCardSlot, int32, CardID);

class UButton;
class UImage;
class UTextBlock;
class UShopCardWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopCardSlot : public UW_ShopCardBase
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
protected:
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> CardImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> CardNameTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> CardPriceTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> BuyCardButton;

public:
	FOnBuyCard OnBuyCard;

protected:

	FCardData WidgetCardData;

public:

	void InitCardSlot(FCardData InitCard);

	void SetCardSlotHidden();
protected:
	UFUNCTION()
	void ClickBuyCardButton();

protected:

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;

};
