// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/GameSettingGISubsystem.h"


void UGameSettingGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameUserSettings = UGameUserSettings::GetGameUserSettings();
}

void UGameSettingGISubsystem::SetScreenResolution(FIntPoint Resolution)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(Resolution);
	}
}

FIntPoint UGameSettingGISubsystem::GetCurrentScreenResolution() const
{
	if (GameUserSettings)
	{
		return GameUserSettings->GetScreenResolution();
	}

	return FIntPoint::ZeroValue;
}

void UGameSettingGISubsystem::SetWindowMode(EWindowMode::Type WindowMode)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetFullscreenMode(WindowMode);
	}
}

EWindowMode::Type UGameSettingGISubsystem::GetCurrentWindowMode() const
{
	if (GameUserSettings)
	{
		return GameUserSettings->GetFullscreenMode();
	}

	return EWindowMode::Fullscreen;
}

void UGameSettingGISubsystem::ApplyAndSaveSettings(bool bCheckForCommandLineOverrides)
{
	if (GameUserSettings)
	{
		// ApplySettings는 설정 적용 후 SaveSettings까지 수행합니다.
		GameUserSettings->ApplySettings(bCheckForCommandLineOverrides);
	}
}
