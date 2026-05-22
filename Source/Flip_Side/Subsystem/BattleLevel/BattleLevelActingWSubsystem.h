// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BossDataTypes.h"
#include "BattleLevelActingWSubsystem.generated.h"

class ACoinActor;
class AUseableItemActor;
class UNiagaraComponent;
class UNiagaraSystem;
class ABossActor;
struct FItemData;
struct FBossPatternBattleData;

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

	FTimerHandle CoinActionRaiseTimer;

	FTimerHandle CoinActionShakeTimer;

	FTimerHandle CoinActionLowerTimer;

	TWeakObjectPtr<ACoinActor> CoinActionActor;

	FSimpleDelegate CoinActionRaiseFinished;

	FSimpleDelegate CoinActionShakeFinished;

	FSimpleDelegate CoinActionLowerFinished;

	FVector CoinActionStartLocation;

	FVector CoinActionTargetLocation;

	FRotator CoinActionStartRotation;

	float CoinActionElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Coin Action")
	float CoinActionRaiseZ = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Coin Action")
	float CoinActionLowerZ = -80.0f;

	UPROPERTY(EditAnywhere, Category = "Coin Action")
	float CoinActionRaiseDuration = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Coin Action")
	float CoinActionShakeDuration = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Coin Action")
	float CoinActionLowerDuration = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Coin Action")
	float CoinActionShakeRoll = 20.0f;

	UPROPERTY()
	class UCoinManagementWSubsystem* CoinManager;

	UPROPERTY()
	class UGridManagerSubsystem* GridManager;

	UPROPERTY()
    TObjectPtr<ABase_PatternVisualActor> CurrentVisualActor = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> PromotionVFXComponent = nullptr;

/* Battle Start */
protected:
	void OpenGrid();

	void DoCoinTeleportAct();

	void MoveCoinsWithDraw();

	void ReadyCoinsMoveWhenLeverDown();

	void TeleportReadyCoinsToDecidedGrid(ACoinActor* ReadyCoin);
public:
	void WaitTeleportUntilLeverDown();

    void AbortForLevelTransition();

	void RaiseCoinForAction(ACoinActor* Coin, FSimpleDelegate OnFinished);

	void ShakeCoinForAction(ACoinActor* Coin, FSimpleDelegate OnFinished);

	void LowerCoinAfterAction(ACoinActor* Coin, FSimpleDelegate OnFinished);

	void StopCoinActionAct();

/* Setting Turn */
public:
	void DoSettingAct();

	void PrepareBossVisualActor(TSoftClassPtr<class ABase_PatternVisualActor> VisualClass);

/* Boss Turn */
public:
	void PlayBossPatternAct();

	// TargetCellLocations: 패턴 적용 그리드 월드 좌표 목록 (TargetCells 모드에서 사용)
	// AnchorLocation: AnchorCell 월드 좌표 (AnchorCell 모드에서 사용)
	void PlayBossVFX(UNiagaraSystem* Effect, EBossPatternTarget TargetMode, FVector PatternScale,
		const TArray<FVector>& TargetCellLocations, FVector AnchorLocation);

/* Useable Item */
public:
	void PlayUseableItemFailedVFX(AUseableItemActor* ItemActor);

	void PlayUseableItemFailedVFXAtActor(AActor* TargetActor);

	void PlayUseableItemVFX(const FItemData& ItemData, class AGridActor* TargetGrid, ACoinActor* TargetCoin, AActor* TargetOther);

	void PlayPhaseChangePotionAct(ACoinActor* TargetCoin);

	void ShowPromotionVFX(const FVector& Location);

	void HidePromotionVFX();
	
/* 연출용 델리게이트 */
public:
	FOnDrawMove DoDrawMove;

	FOnGear DoGearMove;

	FOnGear OnCoinLanded;

	FOnGear OnBossDeadAct;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    virtual void Deinitialize() override;

	void UpdateCoinActionRaise();

	void UpdateCoinActionShake();

	void UpdateCoinActionLower();
};
