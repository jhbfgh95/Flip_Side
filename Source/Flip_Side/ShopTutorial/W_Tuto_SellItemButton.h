// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopItem/W_SellItemButton.h"
#include "W_Tuto_SellItemButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_SellItemButton : public UW_SellItemButton
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 ExcuteOrder;

	UFUNCTION()
	void ExcuteEvent();
};
