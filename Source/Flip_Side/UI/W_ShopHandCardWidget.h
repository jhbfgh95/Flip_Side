// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_CardWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopHandCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopHandCardWidget : public UW_CardWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	class UShopCardWSubsystem* ShopCardSubSystem;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

public:
	UPROPERTY(EditAnywhere)
	int32 HandIndex;
	UFUNCTION()
	void SelectedCard(int32 Index, FCardData CardData);
	void RemoveCard();
};
