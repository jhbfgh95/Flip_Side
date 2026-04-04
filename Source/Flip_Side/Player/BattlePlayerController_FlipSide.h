// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlipSide_Enum.h"
#include "BattlePlayerController_FlipSide.generated.h"

class UInputMappingContext;
class ABattlePlayerPawn_FlipSide;
class ABattleArea;
class IBattleHoverInterface;
class IBattleClickInterface;

UCLASS(abstract)
class ABattlePlayerController_FlipSide : public APlayerController
{
	GENERATED_BODY()
private:
    FVector DefaultCameraLocation;
    FRotator DefaultCameraRotation;
    float DefaultCameraArmLength;

	UPROPERTY(EditAnywhere, Category = "Camera")
    FVector CoinSelectCameraLocation;

    UPROPERTY(EditAnywhere, Category = "Camera")
    FRotator CoinSelectCameraRotation;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CoinSelectCameraArmLength = 0.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CoinSelectCameraDelay = 0.6f;

    FTimerHandle CoinSelectCameraDelayHandle;

    UFUNCTION()
    void OnTurnChanged(ETurnState NewTurn);

protected:
	TObjectPtr<ABattlePlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

	virtual void BeginPlay() override;

	void ReturnToDefaultCamera();

    void OnLeftClick();

	void OnRightClick();

    void CheckMouseHover();

    UPROPERTY()
    class ABattleArea* CurrentHoveredArea;

	UPROPERTY()
    AActor* LastHoveredActor;

public:
	ABattlePlayerController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

	virtual void PlayerTick(float DeltaTime) override; // 매 프레임 마우스 호버 감지용
};
