// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FlipSideGameUserSettings.generated.h"

/**
 * 해상도·창 모드와 같은 GameUserSettings.ini에 저장되는 게임 설정입니다. 전체 볼륨처럼 엔진 기본 설정에 없는 값을 둡니다.
 * DefaultEngine.ini의 GameUserSettingsClassName으로 등록되어 UGameUserSettings::GetGameUserSettings()가 이 클래스를 돌려줍니다.
 */
UCLASS()
class FLIP_SIDE_API UFlipSideGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Settings|Sound")
	int32 GetMasterVolumePercent() const { return MasterVolumePercent; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Sound")
	void SetMasterVolumePercent(int32 InPercent) { MasterVolumePercent = FMath::Clamp(InPercent, 0, 100); }

	static UFlipSideGameUserSettings* GetFlipSideGameUserSettings();

protected:
	// 0~100. 적용은 UGameSettingGISubsystem::ApplyMasterVolume, 저장은 SaveSettings가 합니다.
	UPROPERTY(Config)
	int32 MasterVolumePercent = 100;
};
