// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "W_SelectShopCoinSlotButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_SelectShopCoinSlotButton : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

protected:
	virtual void NativeConstruct() override;
public:

	UPROPERTY(EditAnywhere)
	int32 SlotIndex = 0;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectSlotButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SlotCoinCountText;

	UFUNCTION()
	void ClickSlotButton();

	UFUNCTION()
	void SetSlotCoinCount(int32 CurrentSlotIndex, int32 Count);

	UFUNCTION()
	void InitButton(int32 ChangeCoinSlotIndex, EWeaponClass SlotWeaponIndex);

};
