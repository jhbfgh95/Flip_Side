// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController_FlipSide.generated.h"

class UInputMappingContext;
class ABattlePlayerPawn_FlipSide;
class ABattleArea;

UCLASS(abstract)
class ABattlePlayerController_FlipSide : public APlayerController
{
	GENERATED_BODY()
protected:
	TObjectPtr<ABattlePlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

	// 마우스 좌클릭
    void OnLeftClick();

	// 마우스 우클릭
	void OnRightClick();

    // for 마우스 대상 감지
    void CheckMouseHover();

    // 현재 마우스가 올라가 있는 영역 액터 저장
    UPROPERTY()
    class ABattleArea* CurrentHoveredArea;

public:
	ABattlePlayerController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

	virtual void PlayerTick(float DeltaTime) override; // 매 프레임 마우스 호버 감지용
};
