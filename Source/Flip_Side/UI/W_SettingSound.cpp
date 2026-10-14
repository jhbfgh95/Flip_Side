// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingSound.h"

#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Engine/GameInstance.h"
#include "Subsystem/GameSettingGISubsystem.h"

void UW_SettingSound::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		GameSettingSubsystem = GameInstance->GetSubsystem<UGameSettingGISubsystem>();
	}

	MaximumVolume = FMath::Max(MaximumVolume, MinimumVolume);

	if (IsValid(VolumeSlider))
	{
		VolumeSlider->SetMinValue(static_cast<float>(MinimumVolume));
		VolumeSlider->SetMaxValue(static_cast<float>(MaximumVolume));
		VolumeSlider->SetStepSize(1.0f);
		VolumeSlider->OnValueChanged.AddUniqueDynamic(
			this, &ThisClass::HandleVolumeSliderValueChanged);
		VolumeSlider->OnMouseCaptureEnd.AddUniqueDynamic(
			this, &ThisClass::HandleVolumeSliderCaptureEnd);
	}

	if (IsValid(VolumeValueTextBox))
	{
		VolumeValueTextBox->OnTextCommitted.AddUniqueDynamic(
			this, &ThisClass::HandleVolumeTextCommitted);
	}

	// 저장된 전체 볼륨이 있으면 그 값으로 시작합니다.
	SetVolumeValue(IsValid(GameSettingSubsystem) ? GameSettingSubsystem->GetMasterVolume() : InitialVolume, false);
}

void UW_SettingSound::HandleVolumeSliderValueChanged(float NewValue)
{
	if (!bIsSynchronizingValue)
	{
		SetVolumeValue(FMath::RoundToInt(NewValue), true);
	}
}

void UW_SettingSound::HandleVolumeTextCommitted(const FText& Text, ETextCommit::Type /*CommitMethod*/)
{
	const FString InputText = Text.ToString();
	if (InputText.IsNumeric())
	{
		SetVolumeValue(FCString::Atoi(*InputText), true);
		if (IsValid(GameSettingSubsystem))
		{
			GameSettingSubsystem->SaveMasterVolume();
		}
	}
	else
	{
		// 유효하지 않은 입력은 현재 슬라이더 값으로 되돌립니다.
		SetVolumeValue(VolumeSlider ? FMath::RoundToInt(VolumeSlider->GetValue()) : InitialVolume, false);
	}
}

void UW_SettingSound::HandleVolumeSliderCaptureEnd()
{
	if (IsValid(GameSettingSubsystem))
	{
		GameSettingSubsystem->SaveMasterVolume();
	}
}

void UW_SettingSound::SetVolumeValue(int32 NewValue, bool bBroadcastChange)
{
	const int32 ClampedValue = GetClampedVolumeValue(NewValue);

	bIsSynchronizingValue = true;
	if (IsValid(VolumeSlider))
	{
		VolumeSlider->SetValue(static_cast<float>(ClampedValue));
	}

	if (IsValid(VolumeValueTextBox))
	{
		VolumeValueTextBox->SetText(FText::AsNumber(ClampedValue));
	}
	bIsSynchronizingValue = false;

	if (bBroadcastChange)
	{
		if (IsValid(GameSettingSubsystem))
		{
			GameSettingSubsystem->SetMasterVolume(ClampedValue);
		}
		OnSoundValueChanged.Broadcast(ClampedValue);
	}
}

int32 UW_SettingSound::GetClampedVolumeValue(int32 Value) const
{
	return FMath::Clamp(Value, MinimumVolume, MaximumVolume);
}
