// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CoinCountButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinCountButton : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* CountButton;
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsIncrease = false;
	int32 SlotIndex = 0;

private:
	UFUNCTION()
	void ChangeCount();
public:
	void InitButton(int32 SlotNum);

};
