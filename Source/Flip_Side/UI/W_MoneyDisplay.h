// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_MoneyDisplay.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_MoneyDisplay : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MoneyTextBlock;

	UFUNCTION()
	void SetMoneyText(int32 Money);

private:
	int32 MoneyText = 0;
	int32 CurrentMoneyText;
	bool IsMoneyChanged;
};
