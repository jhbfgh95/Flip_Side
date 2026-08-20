// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopSelectedItem.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class FLIP_SIDE_API UW_ShopSelectedItem : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

public:
	UFUNCTION(BlueprintCallable)
	void SetImage(UTexture2D* InItemTexture);
};
