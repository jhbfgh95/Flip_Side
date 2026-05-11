// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_PriceWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_PriceWidget : public UUserWidget
{
	GENERATED_BODY()
	

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PriceText;

public:
	void SetPriceText(int32 Price);
};
