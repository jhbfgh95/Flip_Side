// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BattleTutorialWSubsystem.generated.h"

class ABattlePlayerController_FlipSide;
class ACoinActor;
class ATutorialTargetPoint;
class UBattleManagerWSubsystem;
class UBattleTutorialSequenceData;
class UCoinManagementWSubsystem;
class UW_BattleTutorialOverlay;

UCLASS()
class FLIP_SIDE_API UBattleTutorialWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void InitBattleTutorial(UBattleTutorialSequenceData* InSequenceData, TSubclassOf<UW_BattleTutorialOverlay> InOverlayClass, int32 ZOrder = 100);

	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void EndBattleTutorial();

	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void AdvanceBattleTutorial();

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

private:
	void CacheTutorialTargets();
	void BindBattleEvents();
	void UnbindBattleEvents();
	void ApplyCurrentStep();
	void HandleOverlayClicked();
	void HandleCoinSlotClicked(ACoinActor* ClickedCoin);
	void HandleLeverTriggered();
	void SetTutorialInput(bool bUIOnly);

	UPROPERTY()
	TObjectPtr<UBattleTutorialSequenceData> SequenceData;

	UPROPERTY()
	TObjectPtr<UW_BattleTutorialOverlay> OverlayWidget;

	UPROPERTY()
	TObjectPtr<UCoinManagementWSubsystem> CoinManager;

	UPROPERTY()
	TObjectPtr<UBattleManagerWSubsystem> BattleManager;

	UPROPERTY()
	TObjectPtr<ABattlePlayerController_FlipSide> BattlePlayerController;

	UPROPERTY()
	TMap<FName, TObjectPtr<ATutorialTargetPoint>> TutorialTargetMap;

	int32 CurrentStepIndex = INDEX_NONE;
	int32 CurrentStepClickCount = 0;
	bool bInitialized = false;
};
