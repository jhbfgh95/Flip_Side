// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/GameSettingGISubsystem.h"

#include "AudioDevice.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/FlipSideGameUserSettings.h"

void UGameSettingGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (IsValid(GameUserSettings))
	{
		// 게임 시작 시 기본 프레임 제한을 적용하며, 레벨 전환 후에도 유지됩니다.
		GameUserSettings->SetFrameRateLimit(60.f);
		GameUserSettings->ApplyNonResolutionSettings();
	}
}

void UGameSettingGISubsystem::SetMasterVolume(int32 VolumePercent)
{
	if (UFlipSideGameUserSettings* Settings = Cast<UFlipSideGameUserSettings>(GameUserSettings))
	{
		Settings->SetMasterVolumePercent(VolumePercent);
	}
	ApplyMasterVolume(GetWorld());
}

void UGameSettingGISubsystem::SaveMasterVolume()
{
	if (IsValid(GameUserSettings))
	{
		GameUserSettings->SaveSettings();
	}
}

int32 UGameSettingGISubsystem::GetMasterVolume() const
{
	const UFlipSideGameUserSettings* Settings = Cast<UFlipSideGameUserSettings>(GameUserSettings);
	return Settings ? Settings->GetMasterVolumePercent() : 100;
}

void UGameSettingGISubsystem::ApplyMasterVolume(const UWorld* World) const
{
	if (!IsValid(World))
	{
		return;
	}

	const float Volume = static_cast<float>(GetMasterVolume()) / 100.f;
	const UFlipSideDevloperSettings* DevSettings = GetDefault<UFlipSideDevloperSettings>();
	USoundMix* MasterMix = DevSettings ? DevSettings->MasterSoundMix.LoadSynchronous() : nullptr;
	USoundClass* MasterClass = DevSettings ? DevSettings->MasterSoundClass.LoadSynchronous() : nullptr;
	if (MasterMix && MasterClass)
	{
		// 사운드 믹스는 레벨이 바뀌어도 오디오 장치에 남습니다. 모든 소리는 프로젝트 기본 사운드 클래스(MasterClass)에 속합니다.
		UGameplayStatics::SetSoundMixClassOverride(World, MasterMix, MasterClass, Volume, 1.f, 0.f, true);
		UGameplayStatics::PushSoundMixModifier(World, MasterMix);
		return;
	}

	// 믹스 에셋이 설정되지 않았을 때의 대비책입니다. 이 값은 레벨이 바뀌면 엔진이 1로 되돌립니다.
	if (FAudioDeviceHandle AudioDevice = World->GetAudioDevice())
	{
		AudioDevice->SetTransientPrimaryVolume(Volume);
	}
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

void UGameSettingGISubsystem::ApplyScreenResolutionSettings(bool bCheckForCommandLineOverrides)
{
	if (GameUserSettings)
	{
		GameUserSettings->ApplyResolutionSettings(bCheckForCommandLineOverrides);
	}
}

void UGameSettingGISubsystem::ApplyAndSaveSettings(bool bCheckForCommandLineOverrides)
{
	if (GameUserSettings)
	{
		// ApplySettings는 설정 적용 후 SaveSettings까지 수행합니다.
		GameUserSettings->ApplySettings(bCheckForCommandLineOverrides);
	}
}
