// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CountWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CountWidget : public UUserWidget
{
	GENERATED_BODY()
	private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TenText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OneText;


public:
	void SetCountText(int32 Count);
};
