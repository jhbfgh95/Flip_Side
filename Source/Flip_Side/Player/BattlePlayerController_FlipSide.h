// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController_FlipSide.generated.h"

class UInputMappingContext;
class ABattlePlayerPawn_FlipSide;

UCLASS(abstract)
class ABattlePlayerController_FlipSide : public APlayerController
{
	GENERATED_BODY()
protected:
	TObjectPtr<ABattlePlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

public:
	ABattlePlayerController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);
};
