// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ShopCoinWSubsystem.generated.h"

#define MAX_TOTAL_COIN 30
/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UShopCoinWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
private:
	//총 코인 개수
	int32 TotalCoinCount;

	TArray<FCoinTypeStructure> CoinArray;
	TArray<EWeaponClass> CoinClassArray;
	//void InitCoinArray();

private:


public:
	bool CanIncreaseCoin(int32 SlotNum);
	bool CanDecreaseCoin(int32 SlotNum);

	void IncreaseSlotCoinCount(int32 SlotNum);
	void DecreaseSlotCoinCount(int32 SlotNum);

	void ResetCoin(int32 SlotNum);


	FCoinTypeStructure GetSlotCoin(int32 SlotNum);
	EWeaponClass GetSlotCoinClass(int32 SlotNum);

	void SetSlotCoin(FCoinTypeStructure SetCoinInfo, EWeaponClass CoinClass);
};
