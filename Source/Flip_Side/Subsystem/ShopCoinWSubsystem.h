// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
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

	//void InitCoinArray();

public:
	bool CanAddCoin(int32 SlotNum);
	bool CanRemoveCoin(int32 SlotNum);
	void AddSlotCoin(int32 SlotNum);
	void RemoveSlotCoin(int32 SlotNum);

	FCoinTypeStructure GetSlotCoin(int32 SlotNum);
	void SetSlotCoin(FCoinTypeStructure SetCoinInfo);
};
