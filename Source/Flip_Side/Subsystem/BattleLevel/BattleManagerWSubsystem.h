// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "BattleManagerWSubsystem.generated.h"

UENUM()
enum class EBattleLeverGateState : uint8
{
    Locked,
    WaitingForLever,
};

UENUM()
enum class EBattleLeverLockReason : uint8
{
    None,
    TurnTransition,
    SettingTurn,
    BossTurn,
    LeverAnimation,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, ETurnState, NewTurn);
//StageEndFlag: 0 = StageClear, 1 = GameOver, 2 = GameClear
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageEnded, int32, StageEndFlag);
DECLARE_MULTICAST_DELEGATE(FOnBattleTutorialLeverTriggered);

class ACoinActor;
class UW_StageEnd;
class UW_BattlePhaseAndTurnDisplayUI;

UCLASS()
class FLIP_SIDE_API UBattleManagerWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<ETurnState> TurnManageMentStack;

	UPROPERTY()
	TArray<FRandomState> RandomStateArray;

	ETurnState CurrentTurn;
	
	bool bIsStageEnded = false;

	int32 TurnCount = 0;

	UPROPERTY()
	UW_StageEnd* StageEndWidgetInstance = nullptr;

	UPROPERTY()
	UW_BattlePhaseAndTurnDisplayUI* BattlePhaseAndTurnDisplayWidgetInstance = nullptr;


/* 레버 잠금 */
protected:
	FTimerHandle LockLeverWhenCanInteractTimer;

	//레버가 작동할 수 있는가?
	EBattleLeverGateState LeverGateState = EBattleLeverGateState::Locked;

	//작동 할 수 없다면 이유는 무엇인가
	EBattleLeverLockReason LeverLockReason = EBattleLeverLockReason::None;
	
	FTimerHandle LeverUnlockTimer;

	float LeverLockTime = 0.0f;

	/*FUNCTION*/
	void LockLever(EBattleLeverLockReason Reason);

    void UnlockLever();

    void UnlockLeverAfter(float DelaySeconds);


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

	UPROPERTY()
	class UStageCardWSubsystem* StageCardManager;

	UPROPERTY()
	class USoundManagerWSubsystem* SoundManager;

protected:
	void TurnProgressing();

	void TurnStackInit();

	void GenerateRandomStates();

/* CoinBehaviorTurn (순서대로) */
protected:
	void MatchCoinsToRandomState();

protected:
	/* CoinReadyTurn */
	void DoCoinReadyTurn();
	/* CoinBehaviorTurn */
	void DoCoinBehaviorTurn();
	/* SettingTurn*/
	UFUNCTION()
	void DoSettingTurn();
	/* BossTurn*/
	void DoBossTurn();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	//스테이지 클리어
	UFUNCTION()
	void StageEnded();

	UFUNCTION()
	void BossDeathStarted();

	UFUNCTION()
	void GameOver();

	bool TryEndStage(int32 StageEndFlag);

	void AddStageClearRefundToMoney();

	void ShowStageEndWidget(int32 StageEndFlag);

public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;

	UPROPERTY(BlueprintAssignable)
	FOnStageEnded OnStageEnded;

	FOnBattleTutorialLeverTriggered OnBattleTutorialLeverTriggered;

	ETurnState GetCurrentTurn();

	bool StartBattleFromLever(float BattleLeverEndTime);

};
