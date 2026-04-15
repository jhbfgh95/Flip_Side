// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopUnlockCoinSlot.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopUnlockCoinSlot : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
private:
	UPROPERTY(meta= (BindWidget))
	class UButton* UnlockCoinSlotButton;
	UFUNCTION()
	void SetWidgetActive(bool ISChangeBottom);
	UFUNCTION()
	void ClickUnlockButton();
};
