// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BattlePlayerPawn_FlipSide.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(abstract)
class ABattlePlayerPawn_FlipSide : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

public:
	ABattlePlayerPawn_FlipSide();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 카메라 이동: 시점 전환
	void MoveCameraToArea(FVector NewTargetLocation, FRotator NewTargetRotation, float NewTargetArmLength);

private:
	// 카메라의 부드러운 이동을 위한 목표 상태 저장
    FVector TargetLocation;      // 목표 위치 (Pawn의 위치)
    FRotator TargetRotation;     // 목표 회전 (SpringArm의 회전)
    float TargetArmLength;       // 목표 거리 (SpringArm의 길이)

    UPROPERTY(EditAnywhere, Category = "Camera Settings")
    float InterpolationSpeed = 5.0f; // 이동 속도
};