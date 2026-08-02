// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ShopCoinViewModel.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCoinSlot, int32, SlotNum);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateCoinSlotCount);

class UShopCoinWSubsystem;
class UDataManagerSubsystem;
class UUnlockGISubsystem;
UCLASS()
class FLIP_SIDE_API UShopCoinViewModel : public UObject
{
	GENERATED_BODY()


public:
	void Initialize();

private:
	UPROPERTY()
	TObjectPtr<UShopCoinWSubsystem> CoinSubsystem;

	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;
	
	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;

public:

	FUpdateCoinSlot OnUpdateCoinSlot;

	FUpdateCoinSlotCount OnUpdateCoinSlotCount;

public:

	void BuyCoinSlot();

	void SellCoinSlot();

	void IncreaseCoinSlotCoin(int32 SlotIndex, int32 Amount);

	void DecreaseCoinSlotCoin(int32 SlotIndex, int32 Amount);

	void SelectCoinSlot();

	void ChangeCoinSlotOrder();

	int32 GetUnlockCoinSlotCount();

	int32 GetCurrentCoinSlotIndex();

	int32 GetCoinSlotCoinCount(int32 SlotNum);

	FCoinTypeStructure GetCoinSlotCoinType(int32 SlotNum);

public:
	void SetWeaponToCoinSide(int32 WeaponID, EWeaponClass WeaponClass);
	
	void ChageCoinSide();

	bool GetIsCoinFront();

public:
	FFaceData GetWeaponData(int32 WeaponID);

	const TArray<int32>& GetUnlockWeaponArray(EWeaponClass WeaponClass);
};
