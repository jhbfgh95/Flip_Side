// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CardSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CardSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* LeftCardButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* RightCardButton;

	
	UPROPERTY(meta = (BindWidget))
	class UImage* CardImage;
};
