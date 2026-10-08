#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BattleTutorialSequenceData.h"
#include "BattleTutorialWSubsystem.generated.h"

class ABattlePlayerController_FlipSide;
class ACoinActor;
class ATutorialTargetPoint;
class UBattleManagerWSubsystem;
class UCoinManagementWSubsystem;
class UW_BattleTutorialOverlay;
class UButton;
class UWidget;

UCLASS()
class FLIP_SIDE_API UBattleTutorialWSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void InitBattleTutorial(UBattleTutorialSequenceData* InSequenceData, TSubclassOf<UW_BattleTutorialOverlay> InOverlayClass, int32 ZOrder = 100);
	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void EndBattleTutorial();
	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void AdvanceBattleTutorial();
	UFUNCTION(BlueprintCallable, Category = "Battle Tutorial")
	void NotifyTargetAction(FName ActionId);
	bool CanAddTutorialCoin(int32 SlotNumber) const;
	bool CanActWithTutorialCoin(const ACoinActor* Coin) const;
	bool CanSelectTutorialCoin(const ACoinActor* Coin) const;
	bool CanSelectTutorialItem(int32 ItemID) const;
	bool CanHoverTutorialCoin(const ACoinActor* Coin) const;
	bool IsTutorialWorldClickAllowed() const;
	bool IsInitialized() const { return bInitialized; }
	bool CanProgressTutorialPhase() const;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bInitialized && !IsTemplate(); }
	virtual TStatId GetStatId() const override;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
private:
	void InitBattleTutorialFromSettings();
	void CacheTutorialTargets();
	void BindBattleEvents();
	void UnbindBattleEvents();
	void ApplyCurrentStep();
	void HandleOverlayClicked();
	void HandleCoinSlotAdded(int32 SlotNumber);
	void HandleLeverTriggered();
	void HandleCoinActionCompleted(ACoinActor* Coin);
	void HandleCoinActionStarted(ACoinActor* Coin);
	void HandleCoinActionFinished(ACoinActor* Coin);
	void HandleItemAnimationFinished();
	UFUNCTION() void HandleBossPatternFinished();
	void ApplyStepExit();
	void CheckHoverStep();
	void HandleItemSelected(int32 ItemID);
	void HandleItemUsed(int32 ItemID);
	UFUNCTION() void HandleActionButtonClicked();
	void QueueAdvance();
	void FinishBattleTutorial();
	UWidget* ResolveWidget(const FString& Path) const;
	ACoinActor* ResolveRuntimeCoin(int32 Index) const;
	void RefreshTarget();
	UPROPERTY() TObjectPtr<UBattleTutorialSequenceData> SequenceData;
	UPROPERTY(Transient) TArray<FBattleTutorialStep> TutorialSteps;
	UPROPERTY() TObjectPtr<UW_BattleTutorialOverlay> OverlayWidget;
	UPROPERTY() TObjectPtr<UCoinManagementWSubsystem> CoinManager;
	UPROPERTY() TObjectPtr<UBattleManagerWSubsystem> BattleManager;
	UPROPERTY() TObjectPtr<ABattlePlayerController_FlipSide> BattlePlayerController;
	UPROPERTY() TMap<FName, TObjectPtr<ATutorialTargetPoint>> TutorialTargetMap;
	TWeakObjectPtr<UButton> ActionButton;
	FTimerHandle InitBattleTutorialTimerHandle;
	FTimerHandle AdvanceTimerHandle;
	int32 CurrentStepIndex = INDEX_NONE;
	int32 CurrentStepClickCount = 0;
	int32 ReadyCountAtStepStart = 0;
	bool bInitialized = false;
	bool bAdvanceQueued = false;
	bool bWaitingForLanding = false;
	bool bWaitingForAbilitySelection = false;
	bool bWaitingForItemAnimation = false;
	bool bItemAnimationFinished = false;
	bool bWaitingForBossPattern = false;
};
