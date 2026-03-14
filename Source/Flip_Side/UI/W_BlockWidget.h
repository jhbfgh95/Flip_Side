// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_BlockWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_BlockWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	
};
