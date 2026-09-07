// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingWidget.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/GameInstance.h"
#include "Subsystem/GameSettingGISubsystem.h"
#include "TimerManager.h"
#include "UI/W_SettingGraphic.h"

void UW_SettingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	if (IsValid(GraphicSettingWidget))
	{
		GraphicSettingWidget->OnResolutionChanged.AddUniqueDynamic(
			this, &ThisClass::HandleResolutionChanged);
	}

	if (IsValid(ResolutionConfirmButton))
	{
		ResolutionConfirmButton->OnClicked.AddUniqueDynamic(
			this, &ThisClass::HandleResolutionConfirmButtonClicked);
	}

	if (IsValid(ResolutionCancelButton))
	{
		ResolutionCancelButton->OnClicked.AddUniqueDynamic(
			this, &ThisClass::HandleResolutionCancelButtonClicked);
	}

	SetResolutionConfirmOverlayVisible(false);

	if (IsValid(CloseButton))
	{
		CloseButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCloseButtonClicked);
	}
    if (IsValid(BackgroundButton))
	{
        BackgroundButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCloseButtonClicked);
    }
}

void UW_SettingWidget::NativeDestruct()
{
	RevertPendingResolution();
	Super::NativeDestruct();
}

void UW_SettingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bHasPendingResolutionChange)
		return;
	UpdateResolutionConfirmationTimerText();
}


void UW_SettingWidget::HandleResolutionChanged(FIntPoint NewResolution)
{
	UGameInstance* GameInstance = GetGameInstance();
	UGameSettingGISubsystem* GameSettingSubsystem = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UGameSettingGISubsystem>()
		: nullptr;
	if (!IsValid(GameSettingSubsystem))
	{
		return;
	}

	if (!bHasPendingResolutionChange)
	{
		PreviousResolution = GameSettingSubsystem->GetCurrentScreenResolution();
	}

	if (NewResolution == PreviousResolution)
	{
		return;
	}

	GameSettingSubsystem->SetScreenResolution(NewResolution);
	GameSettingSubsystem->ApplyScreenResolutionSettings();

	bHasPendingResolutionChange = true;
	SetResolutionConfirmOverlayVisible(true);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ResolutionConfirmTimerHandle,
			this,
			&ThisClass::RevertPendingResolution,
			7.0f,
			false);
	}

	UpdateResolutionConfirmationTimerText();
}

void UW_SettingWidget::HandleResolutionConfirmButtonClicked()
{
	if (!bHasPendingResolutionChange)
	{
		return;
	}

	ClearResolutionConfirmationTimer();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameSettingGISubsystem* GameSettingSubsystem = GameInstance->GetSubsystem<UGameSettingGISubsystem>())
		{
			GameSettingSubsystem->ApplyAndSaveSettings();
		}
	}

	bHasPendingResolutionChange = false;
	SetResolutionConfirmOverlayVisible(false);
}

void UW_SettingWidget::HandleResolutionCancelButtonClicked()
{
	RevertPendingResolution();
}

void UW_SettingWidget::RevertPendingResolution()
{
	if (!bHasPendingResolutionChange)
	{
		return;
	}

	ClearResolutionConfirmationTimer();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameSettingGISubsystem* GameSettingSubsystem = GameInstance->GetSubsystem<UGameSettingGISubsystem>())
		{
			GameSettingSubsystem->SetScreenResolution(PreviousResolution);
			GameSettingSubsystem->ApplyScreenResolutionSettings();
		}
	}

	if (IsValid(GraphicSettingWidget))
	{
		GraphicSettingWidget->SetSelectedResolution(PreviousResolution);
	}

	bHasPendingResolutionChange = false;
	SetResolutionConfirmOverlayVisible(false);
}

void UW_SettingWidget::ClearResolutionConfirmationTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ResolutionConfirmTimerHandle);
	}
}

void UW_SettingWidget::UpdateResolutionConfirmationTimerText()
{
	if (!IsValid(ResolutionConfirmTimerText))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!bHasPendingResolutionChange || !IsValid(World)
		|| !World->GetTimerManager().IsTimerActive(ResolutionConfirmTimerHandle))
	{
		ResolutionConfirmTimerText->SetText(FText::GetEmpty());
		return;
	}

	const float RemainingTime = World->GetTimerManager().GetTimerRemaining(ResolutionConfirmTimerHandle);
	const int32 RemainingSeconds = FMath::Max(0, FMath::CeilToInt(RemainingTime));
	ResolutionConfirmTimerText->SetText(FText::Format(
		NSLOCTEXT("SettingWidget", "ResolutionRevertCountdown", "{0}초 후 원래 해상도로 되돌아갑니다."),
		FText::AsNumber(RemainingSeconds)));
}

void UW_SettingWidget::SetResolutionConfirmOverlayVisible(bool bVisible) const
{
	if (IsValid(ResolutionConfirmOverlay))
	{
		ResolutionConfirmOverlay->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UW_SettingWidget::HandleCloseButtonClicked()
{
	RevertPendingResolution();
	OnCloseClicked.Broadcast();
}
