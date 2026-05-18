// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinManage/W_CoinSlotContainer.h"
#include "W_Tuto_CoinSlotContainer.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_CoinSlotContainer : public UW_CoinSlotContainer
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
