// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CoinSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinSlotWidget : public UUserWidget
{
	GENERATED_BODY()

virtual void NativeConstruct() override;
virtual void NativeDestruct() override;
private:
	class UShopCoinWSubsystem* CoinSubsystem;
	class UDataManagerSubsystem* DataSubsystem;
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* FrontWeaponImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackWeaponImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CoinCountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SlotIndexText;
	
	UPROPERTY(meta = (BindWidget))
	class UW_CoinCountButton* IncreaseButton;
	
	UPROPERTY(meta = (BindWidget))
	class UW_CoinCountButton* DecreaseButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* BackGroundBorder;


private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTexture2D* DefaultsTexture;
	UPROPERTY()
	class UMaterialInstanceDynamic* FrontDynamicMaterial;
	UPROPERTY()
	class UMaterialInstanceDynamic* BackDynamicMaterial;
	UPROPERTY(EditAnywhere,meta =(AllowPrivateAccess))
	int32 SlotIndex;


	UFUNCTION()
	void PressSlotButton();

	UFUNCTION()
	void SetBackGround();

public:
	UFUNCTION()
	void SetWeaponTexture(int32 WeaponID);
	
	UFUNCTION()
	void SetCountText(int32 SlotNum, int32 Count);

	void InitSlot(int32 SlotNum);
};
