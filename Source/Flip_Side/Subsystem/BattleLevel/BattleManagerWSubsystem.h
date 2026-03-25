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
	TArray<FRandomState> RandomStateArray;

	ETurnState CurrentTurn;

protected:
	/* Subsystem Caches */
	/* 배틀매니저 한정으로 타 매니저들 캐시하는 이유 : 중앙 매니저라서 캐시가 더 이득이라고 생각 */
	UPROPERTY()
	class UCoinManagementWSubsystem* CoinManager;

	UPROPERTY()
	class UGridManagerSubsystem* GridManager;

	UPROPERTY()
	class UUseableItemWSubsystem* ItemManager;

	UPROPERTY()
	class UBattleLevelActingWSubsystem* ActingManager;

	UPROPERTY()
	class UCoinActionManagementWSubsystem* CoinActionManager;

	UPROPERTY()
	class UBossManagerSubsystem* BossManager;

protected:
	void TurnProgressing();

	void TurnStackInit();

	void GenerateRandomStates();

/* CoinSelectTurn (순서대로) */
protected:
	void MatchCoinsToRandomState();

protected:
	/* CoinReadyTurn */
	void DoCoinReadyTurn();
	/* CoinSelectTurn */
	void DoCoinSelectTurn();
	/* SettingTurn*/
	UFUNCTION()
	void DoSettingTurn();
	/* BossTurn*/
	void DoBossTurn();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;

	ETurnState GetCurrentTurn();

	void StartBattleFromLever();
};
