// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDataTypes.h"
#include "W_ShopCoinSlot.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;
class UBorder;
class UTexture2D;
class UShopCoinWSubsystem;
class UDataManagerSubsystem;
UCLASS()
class FLIP_SIDE_API UW_ShopCoinSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
private:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> CoinSubsystem;
	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataSubsystem;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FrontWeaponImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackWeaponImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> CoinCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> HPText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> SlotIndexText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UButton> IncreaseButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UButton> DecreaseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UBorder> BackGroundBorder;

protected:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> DefaultsTexture;

	UPROPERTY(EditAnywhere,meta =(AllowPrivateAccess))
	int32 SlotIndex;
	
	UFUNCTION()
	void PressSlotButton();

	UFUNCTION()
	void SetBackGround();

protected:
	void SetFrontWeaponImage(int32 WeaponID);

	void SetBackWeaponImage(int32 WeaponID);

public:
	UFUNCTION()
	void SetWeaponTexture(int32 WeaponID);
	
	UFUNCTION()
	void SetCountText(int32 SlotNum, int32 Count);

	void InitSlot(int32 SlotNum);

	void ResetSlot();

	void SetCoinSlotCoinType(FCoinTypeStructure CurrentCoinData);
protected:
	
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
