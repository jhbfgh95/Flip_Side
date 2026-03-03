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

	bool bItemFlag = false;

public:
	/* 다른 "밖"의 종류는 너무 나중 일이라서.. 만약에 추가되면 마우스 포인터가 영역을 가르는 방식으로 진수쿤이 고생좀 해줘야할듯*/
	void HandleItemClicked(class AUseableItemActor* TargetItem);

	void HandleGridClicked(class AGridActor* TargetGrid);

	//이건 님이 어떤 방식으로 쓸 지는 모르겠음. 다만, CPP에는 초기화 코드를 넣어놓을게
	//Canceled 코드들은 사실 님 마음대로 해도 상관없음 내가 어케 구현할지를 모르니 그냥 아이템, 그리드로 나눠둔거임
	//통합해도 상관은 없음 그리고 작업 끝나면 해당 주석들 지워주는거 부탁할게 @김진수
	void HandleGridCanceled();

	void HandleItemCanceled();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;

	ETurnState GetCurrentTurn();

	void StartBattleFromLever();
};
