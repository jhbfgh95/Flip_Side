// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "ShopCoinPresenter.generated.h"

class UShopCoinWSubsystem;
class UDataManagerSubsystem;
class UW_ShopCoinSlotContainer;
class UW_ShopCoinSlot;
class UW_ShopWeaponSlotContainer;
class UW_ShopCoinWeaponSlot;
class UW_ShopCoinWidget;
class UUnlockGISubsystem;
UCLASS()
class FLIP_SIDE_API UShopCoinPresenter : public UObject
{
	GENERATED_BODY()

public:
	void InitPresenter(UW_ShopCoinWidget* InShopCoinWidget,
		UShopCoinWSubsystem* InCoinSubsystem, UDataManagerSubsystem* InDataManager, 
		UUnlockGISubsystem* InUnlockSubsystem);


protected:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> CoinSubsystem;
	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;
	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;
private:

	UPROPERTY()
	TObjectPtr<UW_ShopCoinWidget> ShopCoinWidget;
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopCoinSlot>> CoinSlotViews;
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopCoinWeaponSlot>> WeaponSlotViews;


protected:
	void InitSlotWidget();
	void InitWeaponSlotWidget();
	void InitSlotBuyButtonWidget();
	void InitSlotSelectCoin();

protected:
	int32 CurrentSelectedSlotIndex = -1;
protected:
	UFUNCTION()
	void SelectSlot(int32 SlotIndex);

	UFUNCTION()
	void SelectSlotFrontWeapon(int32 SlotIndex);

	UFUNCTION()
	void SelectSlotBackWeapon(int32 SlotIndex);

	UFUNCTION()
	void HoverSlot(int32 SlotIndex);

	UFUNCTION()
	void UnhoverSlot();

	UFUNCTION()
	void SelectWeapon(int32 WeaponID);

	UFUNCTION()
	void HoverWeapon(int32 WeaponID);

	UFUNCTION()
	void UnhoverWeapon();

	UFUNCTION()
	void BuySlot(int32 Level);

	UFUNCTION()
	void CloseSlotBuyPopup();

	UFUNCTION()
	void BuyCoinSlotCoin(int32 SlotIndex, int32 Count);

	UFUNCTION()
	void SellCoinSlotCoin(int32 SlotIndex, int32 Count);

	UFUNCTION()
	void AddWeaponSlot(int32 WeaponID);

	UFUNCTION()
	void ReorderCoinSlot(int32 SourceIndex, int32 TargetIndex);

	UFUNCTION()
	void CancelCoinSlotReorder();
	
	FFaceData GetWeaponData(int32 WeaponID);
	void RefreshCoinSlots();
	void UpdateWeaponDescription(const FFaceData& WeaponData);
	void HideWeaponDescription();

	void SetSlectCoinSideData(int32 SlotIndex);
protected:

	bool IsCurrentCoinSideFront;
	
	void SetCoinSideFront(bool SetFront);

	UFUNCTION()
	void ChangeCoinSide();
};
