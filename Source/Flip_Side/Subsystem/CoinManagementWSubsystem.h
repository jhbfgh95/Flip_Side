// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoinDataTypes.h"
#include "Subsystems/Subsystem.h"
#include "CoinManagementWSubsystem.generated.h"

class ACoinActor;

UCLASS()
class FLIP_SIDE_API UCoinManagementWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCoinTypeStructure> CoinSlotArray;

	UPROPERTY()
	TArray<ACoinActor*> BattleReadyCoins;

	int32 BattleReadyCoinNum = 0;

/*
public:
	TArray<FRandomState> RandomStateArray;
*/
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitCoinSlot();

	void BuildRandomState();

	void InstanceCoins();

public:
	//서랍 초기화
	void InitBattleReadyCoin();

	//클릭시 해당 함수 Call
	void AddBattleReadyCoins(ACoinActor* SelectCoinActor);

	// 서랍에 들어간 코인을 다시 클릭 시 취소 로직
    void RemoveBattleReadyCoins(ACoinActor* SelectCoinActor);

	int32 GetBattleReadyCoinNum();

	bool IsCoinInBattleReady(ACoinActor* InCoin) const;

	ACoinActor* GetCoinByName(FString TargetName);

	bool IsCoinIdInBattleReady(int32 TargetID) const;

	void LockCoinReady();
};
