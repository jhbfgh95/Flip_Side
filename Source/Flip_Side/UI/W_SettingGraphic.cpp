// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingGraphic.h"

#include "Components/ComboBoxString.h"
#include "Engine/GameInstance.h"
#include "Internationalization/TextLocalizationManager.h"
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

void UW_SettingGraphic::NativeConstruct()
{
	Super::NativeConstruct();
	FTextLocalizationManager::Get().OnTextRevisionChangedEvent.RemoveAll(this);
	FTextLocalizationManager::Get().OnTextRevisionChangedEvent.AddUObject(this, &ThisClass::RefreshWindowModeOptions);
	RefreshWindowModeOptions();
}

void UW_SettingGraphic::NativeDestruct()
{
	FTextLocalizationManager::Get().OnTextRevisionChangedEvent.RemoveAll(this);
	Super::NativeDestruct();
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
	if (!IsValid(WindowModeComboBox) || bIsRefreshingWindowModeOptions)
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
	default:
		return;
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
	ResolutionComboBox->ClearOptions();
	FIntPoint CurrentResolution = Resolutions[0];
	if (GameSettingSubsystem)
	{
		CurrentResolution = GameSettingSubsystem->GetCurrentScreenResolution();
	}

	SelectedResolutionIndex = Resolutions.Num() - 1;
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
	SelectedWindowMode = GameSettingSubsystem
		? GameSettingSubsystem->GetCurrentWindowMode()
		: EWindowMode::Fullscreen;
	// 기존에 저장된 경계 없는 창 모드는 지원하는 전체 화면 모드로 전환합니다.
	if (SelectedWindowMode == EWindowMode::WindowedFullscreen)
	{
		SelectedWindowMode = EWindowMode::Fullscreen;
		if (IsValid(GameSettingSubsystem))
		{
			GameSettingSubsystem->SetWindowMode(SelectedWindowMode);
			GameSettingSubsystem->ApplyAndSaveSettings();
		}
	}
	RefreshWindowModeOptions();
}

void UW_SettingGraphic::RefreshWindowModeOptions()
{
	if (!IsValid(WindowModeComboBox)) return;
	// 문자열을 다시 넣는 동안 발생하는 선택 이벤트는 실제 설정 변경으로 처리하지 않습니다.
	TGuardValue<bool> RefreshGuard(bIsRefreshingWindowModeOptions, true);
	WindowModeComboBox->ClearOptions();
	WindowModeComboBox->AddOption(NSLOCTEXT("SettingGraphic", "Windowed", "창 모드").ToString());
	WindowModeComboBox->AddOption(NSLOCTEXT("SettingGraphic", "Fullscreen", "전체 화면").ToString());

	int32 SelectedIndex = 1;
	switch (SelectedWindowMode)
	{
	case EWindowMode::Windowed:
		SelectedIndex = 0;
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
