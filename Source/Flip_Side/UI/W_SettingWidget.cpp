// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/GameInstance.h"
#include "Subsystem/GameSettingGISubsystem.h"
#include "UI/W_SettingGraphic.h"

void UW_SettingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(ApplySettingButton))
	{
		ApplySettingButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleApplySettingButtonClicked);
	}

	if (IsValid(CloseButton))
	{
		CloseButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCloseButtonClicked);
	}
    if (IsValid(BackgroundButton))
	{
        BackgroundButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCloseButtonClicked);
    }
}

void UW_SettingWidget::HandleApplySettingButtonClicked()
{
	UW_SettingGraphic* SettingGraphicWidget = Cast<UW_SettingGraphic>(GraphicSettingWidget);
	UGameInstance* GameInstance = GetGameInstance();
	UGameSettingGISubsystem* GameSettingSubsystem = nullptr;
	if (IsValid(GameInstance))
	{
		GameSettingSubsystem = GameInstance->GetSubsystem<UGameSettingGISubsystem>();
	}

	if (!IsValid(SettingGraphicWidget) || !IsValid(GameSettingSubsystem))
	{
		return;
	}

	FIntPoint SelectedResolution;
	if (!SettingGraphicWidget->GetSelectedResolution(SelectedResolution))
	{
		return;
	}

	EWindowMode::Type SelectedWindowMode = EWindowMode::Fullscreen;
	if (SettingGraphicWidget->IsWindowedMode())
	{
		SelectedWindowMode = EWindowMode::Windowed;
	}
	const bool bResolutionChanged = GameSettingSubsystem->GetCurrentScreenResolution() != SelectedResolution;
	const bool bWindowModeChanged = GameSettingSubsystem->GetCurrentWindowMode() != SelectedWindowMode;

	if (!bResolutionChanged && !bWindowModeChanged)
	{
		return;
	}

	if (bResolutionChanged)
	{
		GameSettingSubsystem->SetScreenResolution(SelectedResolution);
	}

	if (bWindowModeChanged)
	{
		GameSettingSubsystem->SetWindowMode(SelectedWindowMode);
	}

	GameSettingSubsystem->ApplyAndSaveSettings();
}

void UW_SettingWidget::HandleCloseButtonClicked()
{
	OnCloseClicked.Broadcast();
}
