// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_WarningWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_WarningWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	void ActiveWarning(FString Text);

	UPROPERTY(meta = (BindWidget))
	class UImage* WarningImage;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* WarningTextAnim;
};
