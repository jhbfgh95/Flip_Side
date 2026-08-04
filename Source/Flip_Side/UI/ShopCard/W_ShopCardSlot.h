// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopCardSlot.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;
class UShopCardWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopCardSlot : public UUserWidget
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

protected:
	UPROPERTY()
	TObjectPtr<UShopCardWSubsystem> CardSubsystem;

protected:

	FCardData WidgetCardData;

public:

	void InitCardSlot(FCardData InitCard, UShopCardWSubsystem* InitCardSubsystem);

	UFUNCTION()
	void BuyCard();


protected:

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;

};
