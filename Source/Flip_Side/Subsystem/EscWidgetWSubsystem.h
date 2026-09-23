// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EscWidgetWSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UEscWidgetWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

private:
	void CreateAndShowESCWidget(UWorld& World);

	void SaveCurrentGame() const;

	UFUNCTION()
	void HandleQuitGameRequested();

	UFUNCTION()
	void HandleMainMenuRequested();

	UPROPERTY()
	TObjectPtr<class UW_ESCWidget> ESCWidgetInstance;
};
