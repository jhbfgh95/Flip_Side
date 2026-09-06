// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BossDataTypes.h"
#include "CoinDataTypes.h"
#include "ItemDataTypes.h"
#include "CardTypes.h"
#include "FlipSide_Enum.h"
#include "BattlePlayerController_FlipSide.generated.h"

class UInputMappingContext;
class ABattlePlayerPawn_FlipSide;
class ABattleArea;
class IBattleHoverInterface;
class IBattleClickInterface;
class UBattlePlayerHUDWidget;
class ABossActor;
class ACoinActor;
class ACoinAttackRangeIndicatorActor;
class AAbilityRangeActor;
class UComponent_Status;
class UInputAction;
struct FInputActionValue;

UCLASS(abstract)
class ABattlePlayerController_FlipSide : public APlayerController
{
	GENERATED_BODY()
private:
    FVector DefaultCameraLocation;
    FRotator DefaultCameraRotation;
    float DefaultCameraArmLength;

	UPROPERTY(EditAnywhere, Category = "Camera")
    FVector CoinBehaviorCameraLocation;

    UPROPERTY(EditAnywhere, Category = "Camera")
    FRotator CoinBehaviorCameraRotation;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CoinBehaviorCameraArmLength = 0.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CoinBehaviorCameraDelay = 0.6f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    FVector BossDeadCameraLocation;

    UPROPERTY(EditAnywhere, Category = "Camera")
    FRotator BossDeadCameraRotation;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float BossDeadCameraArmLength = 0.f;

    FTimerHandle CoinBehaviorCameraDelayHandle;
    FTimerHandle CursorClickResetHandle;
    FTimerHandle BossHUDBindRetryHandle;

    UFUNCTION()
    void OnPhaseChanged(EPhaseState NewPhase);

    UFUNCTION()
    void OnStageEnded(int32 StageEndFlag);

	UFUNCTION()
	void HandleBossPhaseCompleted();

    UFUNCTION()
    void RefreshBattleCoinHUD();

    UFUNCTION()
    void RefreshBattleItemHUD();

    UFUNCTION()
    void RefreshBattleCardHUD();

    void RefreshBattlePhaseHUD();

    void TryBindBossHUD();
    void RefreshBossHUD();
    void HandleBossHUDDataChanged(const FBossHUDData& InData);
    void MoveCameraForBossDead();
    void HandleBattleCoinSlotClicked(int32 SlotNumber);
    void HandleReadyCoinClicked(int32 CoinInstanceID);
    void HandleReadyCoinHovered(int32 CoinInstanceID);
    void HandleReadyCoinUnhovered(int32 CoinInstanceID);
	void HandleBattleItemSlotClicked(int32 ItemID);
	void HandleBattlePhaseProgressClicked();
    void HandleShowAdditionalBuffsStarted(const FInputActionValue& InputActionValue);
    void HandleShowAdditionalBuffsCompleted(const FInputActionValue& InputActionValue);
    void BeginBattleCoinActorHover(ACoinActor* CoinActor);
    void EndBattleCoinActorHover(ACoinActor* ExpectedCoin = nullptr);
    void SpawnBattleRangePreviewActors();
    void RefreshBattleCoinRangePreviews();
    void ShowBattleCoinRangePreviews(ACoinActor* CoinActor);
    void HideBattleCoinRangePreviews(ACoinActor* CoinActor = nullptr);
    void ObserveBattleInfoCoin(ACoinActor* CoinActor);
    void StopObservingBattleInfoCoin();
    void RefreshHoveredBattleCoinInfo();
    void HandleObservedWeaponStatsChanged(const FWeaponStatsChangedEvent& ChangedEvent);
    void HandleObservedStatusEffectsChanged(const FStatusEffectsChangedEvent& ChangedEvent);
    void HandleObservedHPChanged(int32 DeltaHP);
    void HandleObservedMaxHPChanged(int32 DeltaMaxHP);
    void HandleObservedShieldChanged(int32 DeltaShield);
    void HandleObservedCoinDeath(ACoinActor* DeadCoin);
    bool BuildBattleCoinInfoFromActor(ACoinActor* CoinActor, FBattleCoinInfoViewData& OutViewData) const;
    bool BuildBattleCoinInfoFromReadyData(const FReadyCoinData& ReadyCoinData, FBattleCoinInfoViewData& OutViewData) const;
    bool BuildWeaponFaceInfo(
        int32 WeaponID,
        const FResolvedWeaponFaceStats& ResolvedStats,
        const FLinearColor& WeaponColor,
        FBattleWeaponFaceInfoViewData& OutFaceInfo
    ) const;
    void BuildStatusEffectViewData(
        const TArray<FStatusEffectInstance>& StatusEffects,
        TArray<FBattleStatusEffectViewData>& OutStatusEffects
    ) const;
    FBattleCoinSlotViewData BuildCoinSlotViewData(const FBattleCoinSlotData& CoinSlotData) const;
    FBattleReadyCoinViewData BuildReadyCoinViewData(const FReadyCoinData& ReadyCoinData, int32 ReadySlotNumber) const;
	FBattleItemSlotViewData BuildItemSlotViewData(const FBattleItemSlotData& ItemSlotData, bool bCanUse) const;

protected:
	TObjectPtr<ABattlePlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> ShowAdditionalBuffsInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBattlePlayerHUDWidget> BattleHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UBattlePlayerHUDWidget> BattleHUDWidget;

	/** BP_PlayerController에서 직선 몸통·끝 메시를 설정한 자식 클래스를 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Range Preview")
	TSubclassOf<ACoinAttackRangeIndicatorActor> AttackRangeIndicatorClass;

	/** BP_PlayerController에서 윤곽선 Plane ISM을 설정한 자식 클래스를 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Range Preview")
	TSubclassOf<AAbilityRangeActor> AbilityRangeActorClass;

	UPROPERTY(Transient)
	TObjectPtr<ACoinAttackRangeIndicatorActor> AttackRangeIndicatorActor;

	UPROPERTY(Transient)
	TObjectPtr<AAbilityRangeActor> AbilityRangeActor;

	UPROPERTY()
	TObjectPtr<ABossActor> ObservedBoss;

	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ReturnToDefaultCamera();

    void OnLeftClick();

	void OnRightClick();

    void CheckMouseHover();

    UPROPERTY()
    class ABattleArea* CurrentHoveredArea;

	UPROPERTY()
    AActor* LastHoveredActor;

    TWeakObjectPtr<ACoinActor> HoveredBattleCoin;
    TWeakObjectPtr<ACoinActor> ObservedBattleInfoCoin;
    TWeakObjectPtr<UComponent_Status> ObservedBattleInfoStatus;
    int32 HoveredReadyCoinInstanceID = INDEX_NONE;
    bool bShowAdditionalBuffsHeld = false;

	bool bIsUIOnly = false;

public:
	ABattlePlayerController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

	virtual void PlayerTick(float DeltaTime) override; // 매 프레임 마우스 호버 감지용

	bool GetCursorWorldLocationOnPlane(float PlaneZ, FVector& OutWorldLocation) const;

	/** 기존 ReadyCoin을 DB 무기 ID로 만든 초기 상태의 디버그 코인으로 교체합니다. */
	UFUNCTION(Exec)
	void CreateSampleCoin(int32 FrontID, int32 BackID, int32 ReadyCoinSlotNum);

	UFUNCTION(BlueprintCallable)
	void SetInputForTutorial(bool bEnable);
};
