// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingGraphic.h"

#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Engine/GameInstance.h"
#include "Subsystem/GameSettingGISubsystem.h"

void UW_SettingGraphic::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		GameSettingSubsystem = GameInstance->GetSubsystem<UGameSettingGISubsystem>();
	}

	if (IsValid(ResolutionComboBox))
	{
		ResolutionComboBox->OnSelectionChanged.AddUniqueDynamic(
			this, &ThisClass::HandleResolutionSelectionChanged);
		InitializeResolutionOptions();
	}

	if (IsValid(WindowedModeCheckBox))
	{
		bUseWindowedMode = GameSettingSubsystem
			&& GameSettingSubsystem->GetCurrentWindowMode() == EWindowMode::Windowed;

		WindowedModeCheckBox->SetIsChecked(bUseWindowedMode);
		WindowedModeCheckBox->OnCheckStateChanged.AddUniqueDynamic(
			this, &ThisClass::HandleWindowedModeCheckStateChanged);
	}
}

void UW_SettingGraphic::HandleResolutionSelectionChanged(
	FString /*SelectedItem*/, ESelectInfo::Type /*SelectionType*/)
{
	if (!ResolutionComboBox)
	{
		SelectedResolutionIndex = INDEX_NONE;
		return;
	}

	const int32 NewSelectedIndex = ResolutionComboBox->GetSelectedIndex();
	if (Resolutions.IsValidIndex(NewSelectedIndex))
	{
		SelectedResolutionIndex = NewSelectedIndex;
	}
	else
	{
		SelectedResolutionIndex = INDEX_NONE;
	}
}

void UW_SettingGraphic::HandleWindowedModeCheckStateChanged(bool bIsChecked)
{
	bUseWindowedMode = bIsChecked;
}

void UW_SettingGraphic::InitializeResolutionOptions()
{

	FIntPoint CurrentResolution = Resolutions[0];
	if (GameSettingSubsystem)
	{
		CurrentResolution = GameSettingSubsystem->GetCurrentScreenResolution();
	}

	SelectedResolutionIndex = 0;
	for (int32 ResolutionIndex = 0; ResolutionIndex < Resolutions.Num(); ++ResolutionIndex)
	{
		const FIntPoint& Resolution = Resolutions[ResolutionIndex];
		const FString ResolutionText = FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y);
		ResolutionComboBox->AddOption(ResolutionText);
		if (Resolution == CurrentResolution)
		{
			SelectedResolutionIndex = ResolutionIndex;
		}
	}

	ResolutionComboBox->SetSelectedIndex(SelectedResolutionIndex);
}

bool UW_SettingGraphic::GetSelectedResolution(FIntPoint& OutResolution) const
{
	if (!Resolutions.IsValidIndex(SelectedResolutionIndex))
	{
		return false;
	}

	OutResolution = Resolutions[SelectedResolutionIndex];
	return true;
}

bool UW_SettingGraphic::IsWindowedMode() const
{
	return bUseWindowedMode;
}
