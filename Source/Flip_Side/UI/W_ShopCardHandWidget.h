// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopCardHandWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopCardHandWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UW_CardWidget* LeftCardWidget;
	UPROPERTY(meta = (BindWidget))
	class UW_CardWidget* MiddleCardWidget;
	UPROPERTY(meta = (BindWidget))
	class UW_CardWidget* RightCardWidget;

	
	TArray<class UW_CardWidget*> HandCardWidget;
public:

	void AddHandCard(int32 CardIndex, FCardData CardData);
	void RemoveHandCard();
};
