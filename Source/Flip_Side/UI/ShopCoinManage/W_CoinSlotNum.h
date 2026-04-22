// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ShopCoinManage/W_CoinManagePanelWidget.h"
#include "W_CoinSlotNum.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinSlotNum : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	class UShopCoinWSubsystem* CoinSubsystem;

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;

	UFUNCTION()
	void SetSlotNumText();
};
