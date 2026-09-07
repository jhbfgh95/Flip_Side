// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ESCWidget.h"
#include "Subsystem/LevelGISubsystem.h"
#include "UI/W_SettingWidget.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Kismet/KismetSystemLibrary.h"

void UW_ESCWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		LevelGISubsystem = GameInstance->GetSubsystem<ULevelGISubsystem>();
	}

	if (IsValid(QuitGameButton))
	{
		QuitGameButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitGameButtonClicked);
	}

	if (IsValid(QuitConfirmButton))
	{
		QuitConfirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitConfirmButtonClicked);
	}

	if (IsValid(QuitCancelButton))
	{
		QuitCancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitCancelButtonClicked);
	}

	if (IsValid(QuitConfirmOverlay))
	{
		QuitConfirmOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(MainMenuConfirmButton))
	{
		MainMenuConfirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleMainMenuConfirmButtonClicked);
	}

	if (IsValid(MainMenuCancelButton))
	{
		MainMenuCancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleMainMenuCancelButtonClicked);
	}

	if (IsValid(MainMenuConfirmOverlay))
	{
		MainMenuConfirmOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(ContinueGameButton))
	{
		ContinueGameButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleContinueGameButtonClicked);
	}

	if (IsValid(SettingButton))
	{
		SettingButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingButtonClicked);
	}

	if (IsValid(MainMenuButton))
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleMainMenuButtonClicked);
	}

	if(IsValid(SettingWidget))
	{
		SettingWidget->OnCloseClicked.AddDynamic(this, &UW_ESCWidget::CloseSettingWidget);
		SettingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_ESCWidget::HandleQuitGameButtonClicked()
{
	if (IsValid(QuitConfirmOverlay))
	{
		QuitConfirmOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_ESCWidget::HandleQuitConfirmButtonClicked()
{
	OnQuitGameClicked.Broadcast();
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UW_ESCWidget::HandleQuitCancelButtonClicked()
{
	if (IsValid(QuitConfirmOverlay))
	{
		QuitConfirmOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_ESCWidget::HandleContinueGameButtonClicked()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OnContinueGameClicked.Broadcast();
}

void UW_ESCWidget::HandleSettingButtonClicked()
{
	SettingWidget->SetVisibility(ESlateVisibility::Visible);
	OnSettingClicked.Broadcast();
}

void UW_ESCWidget::HandleMainMenuButtonClicked()
{
	if (IsValid(MainMenuConfirmOverlay))
	{
		MainMenuConfirmOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_ESCWidget::HandleMainMenuConfirmButtonClicked()
{
	if (IsValid(LevelGISubsystem))
	{
		LevelGISubsystem->MoveStartLevel();
	}

	OnMainMenuClicked.Broadcast();
}

void UW_ESCWidget::HandleMainMenuCancelButtonClicked()
{
	if (IsValid(MainMenuConfirmOverlay))
	{
		MainMenuConfirmOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_ESCWidget::CloseSettingWidget()
{
	SettingWidget->SetVisibility(ESlateVisibility::Collapsed);
}
