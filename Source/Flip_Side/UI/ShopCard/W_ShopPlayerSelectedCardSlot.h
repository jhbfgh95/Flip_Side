// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerSelectedCardSlot.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;
class UW_ShopPlayerCardSlot;
class UShopCardWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerSelectedCardSlot : public UUserWidget
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
	TObjectPtr<UButton> UnSelectCardButton;

protected:
	UPROPERTY()
	TObjectPtr<UShopCardWSubsystem> CardSubsystem;

	UPROPERTY()
	TObjectPtr<UW_ShopPlayerCardSlot> PlayerCardSlot;

protected:

	FCardData WidgetCardData;

	bool IsSelected = false;

public:

	void InitCardSlot(UShopCardWSubsystem* InitCardSubsystem);

	UFUNCTION()
	void ClickUnSelectCard();
	
	void SetCardSlot(FCardData SetCardData, UW_ShopPlayerCardSlot* ConnectedPlayerCardSlot);

protected:

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;
};
