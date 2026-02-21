// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_CardWidget.h"
#include "W_HandCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_HandCardWidget : public UW_CardWidget
{
	GENERATED_BODY()

	protected:
	
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* HoverCardAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* UnHoverCardAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* AddCardAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* RemoveCardAnim;
	
};
