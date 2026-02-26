// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "BattleManagerWSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, ETurnState, NewTurn);

class ACoinActor;

UCLASS()
class FLIP_SIDE_API UBattleManagerWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<ETurnState> TurnManageMentStack;

	UPROPERTY()
	TArray<FBattleCoinInfo> CoinOrderArray;

	/*
	//코인ID가 키, 코인의 CurrentID가 value
	UPROPERTY()
	TMap<int32, int32> MatchedArray;
	*/
	UPROPERTY()
	TArray<FRandomState> RandomStateArray;

	ETurnState CurrentTurn;

	/* Subsystem Caches */
	/* 배틀매니저 한정으로 타 매니저들 캐시하는 이유 : 중앙 매니저라서 캐시가 더 이득이라고 생각 */
	UPROPERTY()
	class UCoinManagementWSubsystem* CoinManager;

	UPROPERTY()
	class UGridManagerSubsystem* GridManager;

	UPROPERTY()
	class UBattleLevelActingWSubsystem* ActingManager;

protected:
	void TurnProgressing();

	void CoinOrderArrayInit();

	void TurnStackInit();

	void GenerateRandomStates();

/* CoinReadyTurn */	
protected:
	

/* CoinSelectTurn (순서대로) */
protected:
	void MatchCoinsToRandomState();

	void AddCoinsToOrderArray(ACoinActor* TargetCoin);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;

	ETurnState GetCurrentTurn();

	void StartBattleFromLever();
};
