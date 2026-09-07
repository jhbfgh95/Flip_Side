// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingGraphic.h"

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

	if (IsValid(WindowModeComboBox))
	{
		WindowModeComboBox->OnSelectionChanged.AddUniqueDynamic(
			this, &ThisClass::HandleWindowModeSelectionChanged);
		InitializeWindowModeOptions();
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
		if (!bIsSynchronizingResolutionSelection)
		{
			OnResolutionChanged.Broadcast(Resolutions[SelectedResolutionIndex]);
		}
	}
	else
	{
		SelectedResolutionIndex = INDEX_NONE;
	}
}

void UW_SettingGraphic::HandleWindowModeSelectionChanged(
	FString /*SelectedItem*/, ESelectInfo::Type /*SelectionType*/)
{
	if (!WindowModeComboBox)
	{
		return;
	}

	switch (WindowModeComboBox->GetSelectedIndex())
	{
	case 0:
		SelectedWindowMode = EWindowMode::Windowed;
		break;
	case 1:
		SelectedWindowMode = EWindowMode::Fullscreen;
		break;
	case 2:
		SelectedWindowMode = EWindowMode::WindowedFullscreen;
		break;
	default:
		break;
	}

	if (IsValid(GameSettingSubsystem)
		&& GameSettingSubsystem->GetCurrentWindowMode() != SelectedWindowMode)
	{
		GameSettingSubsystem->SetWindowMode(SelectedWindowMode);
		GameSettingSubsystem->ApplyAndSaveSettings();
	}
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

void UW_SettingGraphic::InitializeWindowModeOptions()
{
	WindowModeComboBox->ClearOptions();
	WindowModeComboBox->AddOption(TEXT("창 모드"));
	WindowModeComboBox->AddOption(TEXT("전체 화면"));
	WindowModeComboBox->AddOption(TEXT("경계없는 창모드"));

	SelectedWindowMode = GameSettingSubsystem
		? GameSettingSubsystem->GetCurrentWindowMode()
		: EWindowMode::Fullscreen;

	int32 SelectedIndex = 1;
	switch (SelectedWindowMode)
	{
	case EWindowMode::Windowed:
		SelectedIndex = 0;
		break;
	case EWindowMode::WindowedFullscreen:
		SelectedIndex = 2;
		break;
	case EWindowMode::Fullscreen:
	default:
		break;
	}

	WindowModeComboBox->SetSelectedIndex(SelectedIndex);
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

void UW_SettingGraphic::SetSelectedResolution(FIntPoint Resolution)
{
	const int32 ResolutionIndex = Resolutions.IndexOfByKey(Resolution);
	if (!Resolutions.IsValidIndex(ResolutionIndex))
	{
		return;
	}

	SelectedResolutionIndex = ResolutionIndex;
	if (IsValid(ResolutionComboBox))
	{
		bIsSynchronizingResolutionSelection = true;
		ResolutionComboBox->SetSelectedIndex(SelectedResolutionIndex);
		bIsSynchronizingResolutionSelection = false;
	}
}

EWindowMode::Type UW_SettingGraphic::GetSelectedWindowMode() const
{
	return SelectedWindowMode;
}
