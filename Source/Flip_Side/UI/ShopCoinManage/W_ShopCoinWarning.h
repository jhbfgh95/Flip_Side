// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/W_WarningWidget.h"
#include "W_ShopCoinWarning.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopCoinWarning : public UW_WarningWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:

	class UShopCoinWSubsystem* CoinSubsystem;

	UFUNCTION()
	void SetWarning(int32 WarningCode);

	
};
