// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingSound.h"

#include "Components/EditableTextBox.h"
#include "Components/Slider.h"

void UW_SettingSound::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MaximumVolume = FMath::Max(MaximumVolume, MinimumVolume);

	if (IsValid(VolumeSlider))
	{
		VolumeSlider->SetMinValue(static_cast<float>(MinimumVolume));
		VolumeSlider->SetMaxValue(static_cast<float>(MaximumVolume));
		VolumeSlider->SetStepSize(1.0f);
		VolumeSlider->OnValueChanged.AddUniqueDynamic(
			this, &ThisClass::HandleVolumeSliderValueChanged);
	}

	if (IsValid(VolumeValueTextBox))
	{
		VolumeValueTextBox->OnTextCommitted.AddUniqueDynamic(
			this, &ThisClass::HandleVolumeTextCommitted);
	}

	SetVolumeValue(InitialVolume, false);
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
	}
	else
	{
		// 유효하지 않은 입력은 현재 슬라이더 값으로 되돌립니다.
		SetVolumeValue(VolumeSlider ? FMath::RoundToInt(VolumeSlider->GetValue()) : InitialVolume, false);
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
		OnSoundValueChanged.Broadcast(ClampedValue);
	}
}

int32 UW_SettingSound::GetClampedVolumeValue(int32 Value) const
{
	return FMath::Clamp(Value, MinimumVolume, MaximumVolume);
}
