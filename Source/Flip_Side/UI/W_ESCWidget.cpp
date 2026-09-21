// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ESCWidget.h"
#include "Subsystem/LevelGISubsystem.h"
#include "UI/W_SettingWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Kismet/KismetSystemLibrary.h"


bool UW_ESCWidget::CloseESCWidget()
{
	CloseCurrentOpenWidget();
	if (IsValid(EscWidget))
	{
		EscWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	return true;
}

void UW_ESCWidget::ToggleESCWidget()
{
	if (IsESCWidgetOpen())
	{
		CloseESCWidget();
		return;
	}

	if (IsValid(EscWidget))
	{
		EscWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

bool UW_ESCWidget::IsESCWidgetOpen() const
{
	return IsValid(EscWidget) && EscWidget->GetVisibility() == ESlateVisibility::Visible;
}



void UW_ESCWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		LevelGISubsystem = GameInstance->GetSubsystem<ULevelGISubsystem>();
	}

	if (IsValid(QuitGameButton))
		QuitGameButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitGameButtonClicked);

	if (IsValid(BackgroundCloseButton))
		BackgroundCloseButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackgroundCloseButtonClicked);

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

	if (IsValid(SettingToggleButton))
	{
		SettingToggleButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingToggleButtonClicked);
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
		SettingWidget->OnCloseClicked.AddUniqueDynamic(this, &ThisClass::CloseCurrentOpenWidget);
		SettingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(EscWidget)) EscWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(MainMenuConfirmOverlay)) MainMenuConfirmOverlay->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(QuitConfirmOverlay)) QuitConfirmOverlay->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ESCWidget::HandleQuitGameButtonClicked()
{
	OpenWidget(QuitConfirmOverlay);
}

void UW_ESCWidget::HandleBackgroundCloseButtonClicked()
{
	CloseESCWidget();
}

void UW_ESCWidget::HandleQuitConfirmButtonClicked()
{
	OnQuitGameClicked.Broadcast();
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UW_ESCWidget::HandleQuitCancelButtonClicked()
{
	CloseCurrentOpenWidget();
}

void UW_ESCWidget::HandleContinueGameButtonClicked()
{
	CloseESCWidget();
	//OnContinueGameClicked.Broadcast();
}

void UW_ESCWidget::HandleSettingToggleButtonClicked()
{
	ToggleESCWidget();
}

void UW_ESCWidget::HandleSettingButtonClicked()
{
	OpenWidget(SettingWidget);
	OnSettingClicked.Broadcast();
}

void UW_ESCWidget::HandleMainMenuButtonClicked()
{
	OpenWidget(MainMenuConfirmOverlay);
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
	CloseCurrentOpenWidget();
}

void UW_ESCWidget::CloseCurrentOpenWidget()
{
	if (!IsValid(CurrentOpenWidget))
	{
		return;
	}
	if(CurrentOpenWidget == SettingWidget)
	{
		SettingWidget->CloseSettingWidget();
	}
	CurrentOpenWidget->SetVisibility(ESlateVisibility::Collapsed);
	CurrentOpenWidget = nullptr;
}

void UW_ESCWidget::OpenWidget(UWidget* WidgetToOpen)
{
	if (!IsValid(WidgetToOpen))
	{
		return;
	}

	// 하나의 팝업만 표시한다. 이미 표시 중인 팝업이면 닫았다가 다시 열지 않는다.
	if (CurrentOpenWidget != WidgetToOpen)
	{
		CloseCurrentOpenWidget();
		CurrentOpenWidget = WidgetToOpen;
	}

	CurrentOpenWidget->SetVisibility(ESlateVisibility::Visible);
}
