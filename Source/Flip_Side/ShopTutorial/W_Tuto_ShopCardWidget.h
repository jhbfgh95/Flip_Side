// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCard/W_ShopCardWidget.h"
#include "W_Tuto_ShopCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_ShopCardWidget : public UW_ShopCardWidget
{
	GENERATED_BODY()
	
	virtual void NativeOnInitialized() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 ExcuteOrder;

	virtual void LeftClickCard() override;
};
