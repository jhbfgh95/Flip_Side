// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerCardSlot.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;
class UShopCardWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerCardSlot : public UUserWidget
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
	TObjectPtr<UButton> SelectCardButton;

protected:
	UPROPERTY()
	TObjectPtr<UShopCardWSubsystem> CardSubsystem;

protected:

	FCardData WidgetCardData;

	bool IsSelected = false;

public:

	void InitCardSlot(FCardData InitCard, UShopCardWSubsystem* InitCardSubsystem);

	UFUNCTION()
	void SelectCard();

	void SetCardSlot(FCardData SetCardData);

	int32 GetSlotCardID();

	void SetSlotIsSelected(bool IsSelected);
protected:

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;
};
