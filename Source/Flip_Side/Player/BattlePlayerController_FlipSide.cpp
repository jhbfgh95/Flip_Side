// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "BattlePlayerPawn_FlipSide.h"

ABattlePlayerController_FlipSide::ABattlePlayerController_FlipSide()
{
    bShowMouseCursor = true;
}

void ABattlePlayerController_FlipSide::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void ABattlePlayerController_FlipSide::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<ABattlePlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}