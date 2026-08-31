#pragma once

#include "CoreMinimal.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/GridTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoinActionManagementWSubsystem.generated.h"

class ABase_OtherActor;
class ABossActor;
class ACoinActor;
class AGridActor;
class UActionLogicRegistryGISubsystem;
class UBattleLevelActingWSubsystem;
class UGridManagerSubsystem;
class UWeapon_Action;
struct FRegisteredAbilityLogic;
struct FWeaponLogicSet;

/** 한 코인 행동 안에서 공격과 각 능력 타이밍을 순서대로 진행합니다. */
enum class ECoinWeaponPipelineStage : uint8
{
	None,
	BeforeAttack,
	Attack,
	OnHit,
	AfterAttackAlways,
	Finishing
};

UCLASS()
class FLIP_SIDE_API UCoinActionManagementWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SetPhase(bool bIsPhase);
	void StopActionSequenceForStageEnd();
	bool IsActionSequenceActive() const { return bActionSequenceActive; }

	UFUNCTION()
	void SetSelectedWeapon(ACoinActor* HoveredCoin);

	UFUNCTION()
	void ExecuteSelectedWeapon(ACoinActor* ClickedCoin);

	UFUNCTION()
	void HandleCoinUnHovered();

	bool TryExecuteOtherAction(ABase_OtherActor* TargetOther);

	UFUNCTION()
	void CancelSingleCellAction(ACoinActor* ClickedCoin);

	void TryCancelCurrentAction();
	void CancelSelectWeapon();

	EActionInputState CurrentInputState = EActionInputState::None;

	UPROPERTY(EditAnywhere, Category = "Coin Action|VFX")
	float CoinNaiagaraTime = 0.5f;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	void ResetActionState(bool bResetSelectedAction = true);
	bool RebuildSelectedAction(ACoinActor* CasterCoin);
	bool RefreshSnapshotIfStale();
	bool RefreshRangeTargets();
	const FWeaponLogicSet* GetCurrentLogicSet() const;
	UActionLogicRegistryGISubsystem* GetLogicRegistry() const;
	UBattleLevelActingWSubsystem* GetActingManager() const;

	void StartCoinActionSequence();
	void BeginRaisedAction();
	void AdvancePipeline();
	bool AdvanceAbilitiesForTiming(EAbilityTiming Timing);
	void BeginAttackStep();
	void ResolveAttackStep();
	void ExecuteAbilityImmediately(const FRegisteredAbilityLogic& AbilityLogic);
	bool BeginManualAbilitySelection(const FRegisteredAbilityLogic& AbilityLogic);
	void CompleteManualAbilitySelection();
	void FinishCoinActionSequence();
	void HandleCoinActionLowerFinished();

	void BuildValidAbilityTargets(const FAbilityTargetRule& Rule);
	void ClearValidAbilityTargets();
	bool IsValidCoinTarget(const ACoinActor* Coin, const FAbilityTargetRule& Rule) const;
	bool IsValidOtherTarget(const ABase_OtherActor* Other, const FAbilityTargetRule& Rule) const;
	int32 ResolveRepeatCount(ERepeatCountSource Source) const;
	void ConfigureInputForRule(const FAbilityTargetRule& Rule);

	UFUNCTION()
	void ExecuteGridAction(AGridActor* TargetGrid);

	void PlayCoinSpecificVFX();
	void PlayCommonVFX(const FWeaponAttackResult& AttackResult);
	void PlayFailedVFX();
	void SpawnVFXAtLocation(class UNiagaraSystem* VFX, const FVector& Location) const;
	void ClearBossOutline();

	bool bIsCorrectPhase = false;
	bool bActionSequenceActive = false;
	bool bPendingFailedVFX = false;
	ECoinWeaponPipelineStage PipelineStage = ECoinWeaponPipelineStage::None;
	int32 CurrentAbilityIndex = 0;
	int32 PendingAbilityIndex = INDEX_NONE;
	int32 PendingSelectionCount = 0;
	int32 RemainingAttackCount = 0;

	FTimerHandle CommonVFXTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<UWeapon_Action> SelectedAction = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UGridManagerSubsystem> GridManager = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACoinActor>> ValidTargetCoins;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AGridActor>> ValidTargetGrids;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ABase_OtherActor>> ValidTargetOthers;
};
