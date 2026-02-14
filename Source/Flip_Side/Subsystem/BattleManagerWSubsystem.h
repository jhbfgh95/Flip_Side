// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "BattleManagerWSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, ETurnState, NewTurn);

UCLASS()
class FLIP_SIDE_API UBattleManagerWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<ETurnState> TurnManageMentStack;

	UPROPERTY()
	TArray<FBattleCoinInfo> CoinOrderArray;

	//코인ID가 키, 코인의 CurrentID가 value
	UPROPERTY()
	TMap<int32, int32> MatchedArray;

	UPROPERTY()
	TArray<FRandomState> RandomStateArray;

	ETurnState CurrentTurn;

protected:
	void TurnProgressing();

	void CoinOrderArrayInit();

	void TurnStackInit();

	void GenerateRandomStates();

/* CoinReadyTurn */	
protected:
	

/* CoinSelectTurn */
protected:
	void MatchCoinsToRandomState();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;

	ETurnState GetCurrentTurn();

	void StartBattleFromLever();
};
