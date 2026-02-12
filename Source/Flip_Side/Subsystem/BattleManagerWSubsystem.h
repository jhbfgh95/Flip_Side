// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "BattleManagerWSubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API UBattleManagerWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<ETurnState> TurnManageMentStack;

	UPROPERTY()
	TArray<FBattleCoinInfo> CoinOrderArray;

	ETurnState CurrentTurn;

protected:
	void TurnProgressing();

	void CoinOrderArrayInit();

	void TurnStackInit();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	ETurnState GetCurrentTurn();

	void StartBattleFromLever();
};
