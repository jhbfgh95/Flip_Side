// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_ShopCoinSlot.generated.h"

/**
 * 
 */
class UButton;
class UImage;
class UTextBlock;
class UBorder;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedShopCoinSlot, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredShopCoinSlot, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredShopCoinSlot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedShopCoinSlotFrontCoin, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedShopCoinSlotBackCoin, int32, SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuyShopCoinSlotCoin, int32, SlotIndex, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSellShopCoinSlotCoin, int32, SlotIndex, int32, Count);

UCLASS()
class FLIP_SIDE_API UW_ShopCoinSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FrontWeaponImageButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackWeaponImageButton;

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

public:
	
	FOnClickedShopCoinSlot OnClickedShopCoinSlot;
	FOnHoveredShopCoinSlot OnHoveredShopCoinSlot;
	FOnUnhoveredShopCoinSlot OnUnhoveredShopCoinSlot;
	FOnClickedShopCoinSlotFrontCoin OnClickedShopCoinSlotFrontCoin;
	FOnClickedShopCoinSlotBackCoin OnClickedShopCoinSlotBackCoin;
	FOnBuyShopCoinSlotCoin OnBuyShopCoinSlotCoin;
	FOnSellShopCoinSlotCoin OnSellShopCoinSlotCoin;

protected:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> DefaultsTexture;

	UPROPERTY(EditAnywhere,meta =(AllowPrivateAccess))
	int32 SlotIndex;
	
	
protected:
	UFUNCTION()
	void ClickFrontWeaponButton();

	UFUNCTION()
	void ClickBackWeaponButton();

	UFUNCTION()
	void PressSlotButton();

	UFUNCTION()
	void BuyCoin();

	UFUNCTION()
	void SellCoin();

	UFUNCTION()
	void SetBackGround();

public:

	void InitSlotWidget(int32 InSlotIndex);

	void SetCoinSlot(int32 Hp);

	void SetFrontWeaponImage(FFaceData InFrontCoinData);

	void SetBackWeaponImage(FFaceData InBackCoinData);

	void SetCountText(int32 Count);

	void ResetSlot();

protected:
	
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
