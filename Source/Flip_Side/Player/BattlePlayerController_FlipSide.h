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
    void OnTurnChanged(ETurnState NewTurn);

    UFUNCTION()
    void OnStageEnded(int32 StageEndFlag);

    UFUNCTION()
    void RefreshBattleCoinHUD();

    UFUNCTION()
    void RefreshBattleItemHUD();

    UFUNCTION()
    void RefreshBattleCardHUD();

    void TryBindBossHUD();
    void RefreshBossHUD();
    void HandleBossHUDDataChanged(const FBossHUDData& InData);
    void MoveCameraForBossDead();
    void HandleBattleCoinSlotClicked(int32 SlotNumber);
    void HandleReadyCoinClicked(int32 CoinInstanceID);
	void HandleBattleItemSlotClicked(int32 ItemID);
    FBattleCoinSlotViewData BuildCoinSlotViewData(const FBattleCoinSlotData& CoinSlotData) const;
    FBattleReadyCoinViewData BuildReadyCoinViewData(const FReadyCoinData& ReadyCoinData, int32 ReadySlotNumber) const;
	FBattleItemSlotViewData BuildItemSlotViewData(const FBattleItemSlotData& ItemSlotData, bool bCanUse) const;

protected:
	TObjectPtr<ABattlePlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBattlePlayerHUDWidget> BattleHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UBattlePlayerHUDWidget> BattleHUDWidget;

	UPROPERTY()
	TObjectPtr<ABossActor> ObservedBoss;

	virtual void BeginPlay() override;

	void ReturnToDefaultCamera();

    void OnLeftClick();

	void OnRightClick();

    void CheckMouseHover();

    UPROPERTY()
    class ABattleArea* CurrentHoveredArea;

	UPROPERTY()
    AActor* LastHoveredActor;

	bool bIsUIOnly = false;

public:
	ABattlePlayerController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

	virtual void PlayerTick(float DeltaTime) override; // 매 프레임 마우스 호버 감지용

	bool GetCursorWorldLocationOnPlane(float PlaneZ, FVector& OutWorldLocation) const;

	UFUNCTION(BlueprintCallable)
	void SetInputForTutorial(bool bEnable);
};
