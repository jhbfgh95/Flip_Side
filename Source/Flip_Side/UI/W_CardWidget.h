// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CardWidget : public UUserWidget
{
	GENERATED_BODY()
	

	private:
	UPROPERTY(meta = (BindWidget))
	class UBorder* CardImage;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CardIconImage;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CardTitle;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CardExplainText;
};
