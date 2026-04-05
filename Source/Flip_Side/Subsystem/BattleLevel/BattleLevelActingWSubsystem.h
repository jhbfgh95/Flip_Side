// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BattleLevelActingWSubsystem.generated.h"

class ACoinActor;

DECLARE_DELEGATE_OneParam(FOnDrawMove, bool);
DECLARE_DELEGATE(FOnGear);

class ABase_PatternVisualActor;

UCLASS()
class FLIP_SIDE_API UBattleLevelActingWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	FTimerHandle LeverDownTimer;

	FTimerHandle LeverWaitTimer;

	FTimerHandle CoinTeleportTimer;

	UPROPERTY()
	class UCoinManagementWSubsystem* CoinManager;

	UPROPERTY()
	class UGridManagerSubsystem* GridManager;

	UPROPERTY()
    TObjectPtr<ABase_PatternVisualActor> CurrentVisualActor = nullptr;

/* Battle Start */
protected:
	void OpenGrid();

	void DoCoinTeleportAct();

	void MoveCoinsWithDraw();

	void ReadyCoinsMoveWhenLeverDown();

	void TeleportReadyCoinsToDecidedGrid(ACoinActor* ReadyCoin);
public:
	void WaitTeleportUntilLeverDown();

/* Setting Turn */
public:
	void DoSettingAct();

	void PrepareBossVisualActor(TSoftClassPtr<class ABase_PatternVisualActor> VisualClass);

/* Boss Turn */
public:
	void PlayBossPatternAct();
	
/* 연출용 델리게이트 */
public:
	FOnDrawMove DoDrawMove;

	FOnGear DoGearMove;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
