// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinManage/W_CoinSlotWidget.h"
#include "W_Tuto_CoinSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_CoinSlotWidget : public UW_CoinSlotWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 ExcuteOrder;

	UFUNCTION()
	void ExcuteEvent();
};
