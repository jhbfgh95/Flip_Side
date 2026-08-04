// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCardSlotContainer.generated.h"

/**
 * 
 */
class UVerticalBox;
class UW_ShopCardSlot;
class UShopCardWSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopCardSlotContainer : public UUserWidget
{
	GENERATED_BODY()

	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
private:
	UPROPERTY()
	TObjectPtr<UShopCardWSubsystem> CardSubsystem;

protected:

	TArray<UW_ShopCardSlot*> ShopCardSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> ShopCardSlotWidget;
};
