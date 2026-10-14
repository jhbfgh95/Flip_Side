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

	/** 해상도 변경을 저장하지 않고 화면에만 적용합니다. */
	UFUNCTION(BlueprintCallable, Category = "Game Settings|Display")
	void ApplyScreenResolutionSettings(bool bCheckForCommandLineOverrides = false);

	UFUNCTION(BlueprintCallable, Category = "Game Settings|Display")
	void ApplyAndSaveSettings(bool bCheckForCommandLineOverrides = false);

	/** 전체 볼륨(0~100)을 바로 적용합니다. 저장은 SaveMasterVolume에서 합니다. */
	UFUNCTION(BlueprintCallable, Category = "Game Settings|Sound")
	void SetMasterVolume(int32 VolumePercent);

	/** 전체 볼륨을 GameUserSettings.ini에 저장합니다. 슬라이더를 놓거나 값을 입력했을 때 부릅니다. */
	UFUNCTION(BlueprintCallable, Category = "Game Settings|Sound")
	void SaveMasterVolume();

	UFUNCTION(BlueprintPure, Category = "Game Settings|Sound")
	int32 GetMasterVolume() const;

	/** 저장된 전체 볼륨을 이 월드의 오디오에 적용합니다. 사운드 믹스로 걸어서 레벨이 바뀌어도 유지됩니다. */
	void ApplyMasterVolume(const UWorld* World) const;

private:
	UPROPERTY()
	TObjectPtr<UGameUserSettings> GameUserSettings;
};
