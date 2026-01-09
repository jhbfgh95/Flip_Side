// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopWidget.generated.h"

class UButton;

UCLASS(abstract)
class UW_ShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FString> BattleLevelNames;
	
protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Button_NextLevel;

	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleNextClicked();

};
