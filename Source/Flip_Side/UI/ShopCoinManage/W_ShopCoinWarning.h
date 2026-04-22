// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCoinWarning.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopCoinWarning : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:

	class UShopCoinWSubsystem* CoinSubsystem;

	UFUNCTION()
	void ActiveWarning(int32 WarningCode);


	
	UPROPERTY(meta = (BindWidget))
	class UImage* LockWarningImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* WarningImage;
	
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* WarningTextAnim;
	
};
