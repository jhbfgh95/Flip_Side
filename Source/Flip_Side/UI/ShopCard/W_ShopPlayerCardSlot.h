// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ShopCard/W_ShopCardBase.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerCardSlot.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectShopPlayerCard, UW_ShopPlayerCardSlot*, SelectedCardSlot, int32, CardID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnselectShopPlayerCard, int32, CardID);

class UButton;
class UImage;
class UTextBlock;
class UShopCardWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerCardSlot : public UW_ShopCardBase
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

public:
	FOnSelectShopPlayerCard OnSelectShopPlayerCard;
	FOnUnselectShopPlayerCard OnUnselectShopPlayerCard;
protected:

	FCardData WidgetCardData;

	bool IsSelected = false;

public:

	void InitCardSlot(FCardData InitCard);

	UFUNCTION()
	void ClickCardButton();

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
