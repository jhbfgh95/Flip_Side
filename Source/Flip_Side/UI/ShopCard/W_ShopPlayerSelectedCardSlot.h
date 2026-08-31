// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ShopCard/W_ShopCardBase.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerSelectedCardSlot.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnselectedPlayerCard, int32, SlotIndex);

class UImage;
class UTextBlock;
class UW_ShopPlayerCardSlot;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerSelectedCardSlot : public UW_ShopCardBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> CardImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> CardNameTextBlock;

public:
	FOnUnselectedPlayerCard OnUnselectedPlayerCard;
protected:

	UPROPERTY()
	TObjectPtr<UW_ShopPlayerCardSlot> PlayerCardSlot;

protected:

	FCardData WidgetCardData;
	int32 SlotIndex = -1;
	bool IsSelected = false;

public:

	void InitCardSlot(int32 InIndex);

	UFUNCTION()
	void ClickUnSelectCard();
	
	void SetCardSlot(FCardData SetCardData, UW_ShopPlayerCardSlot* ConnectedPlayerCardSlot);

	void DeactiveSlot();
	void ClearSlot();

	void UpdateSlot();

	FCardData GetCardData();	
	UW_ShopPlayerCardSlot* GetConnnectedSlot();
protected:

    virtual void NativeOnMouseEnter(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;

    virtual void NativeOnMouseLeave(
        const FPointerEvent& InMouseEvent
    ) override;

    virtual FReply NativeOnMouseButtonDown(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent
    ) override;
};
