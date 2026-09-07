// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSettingGISubsystem.generated.h"

/**
 *
 */
UCLASS()
class FLIP_SIDE_API UGameSettingGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Game Settings|Display")
	void SetScreenResolution(FIntPoint Resolution);

	UFUNCTION(BlueprintPure, Category = "Game Settings|Display")
	FIntPoint GetCurrentScreenResolution() const;

	UFUNCTION(BlueprintCallable, Category = "Game Settings|Display")
	void SetWindowMode(EWindowMode::Type WindowMode);

	UFUNCTION(BlueprintPure, Category = "Game Settings|Display")
	EWindowMode::Type GetCurrentWindowMode() const;

	UFUNCTION(BlueprintCallable, Category = "Game Settings|Display")
	void ApplyAndSaveSettings(bool bCheckForCommandLineOverrides = false);

private:
	UPROPERTY()
	TObjectPtr<UGameUserSettings> GameUserSettings;
};
