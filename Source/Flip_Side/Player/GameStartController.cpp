// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameStartController.h"

#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Subsystem/SaveGISubsystem.h"
#include "UI/W_GameStartWidget.h"
#include "UI/W_SettingWidget.h"

void AGameStartController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	if (GameStartButtonBoxClass)
	{
		GameStartButtonBox = CreateWidget<UW_GameStartWidget>(this, GameStartButtonBoxClass);
		if (IsValid(GameStartButtonBox))
		{
			GameStartButtonBox->OnGameStartRequested.AddUniqueDynamic(this, &ThisClass::HandleGameStartRequested);
			GameStartButtonBox->OnGameLoadRequested.AddUniqueDynamic(this, &ThisClass::HandleGameLoadRequested);
			GameStartButtonBox->OnGameSettingRequested.AddUniqueDynamic(this, &ThisClass::HandleGameSettingRequested);
			GameStartButtonBox->OnGameTutorialRequested.AddUniqueDynamic(this, &ThisClass::HandleGameTutorialRequested);
			GameStartButtonBox->OnGameQuitRequested.AddUniqueDynamic(this, &ThisClass::HandleGameQuitRequested);

			if (USaveGISubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<USaveGISubsystem>())
			{
				if (!SaveSubsystem->DoesSavedGameExist())
				{
					GameStartButtonBox->CollapseLoadButton();
				}
			}

			GameStartButtonBox->AddToViewport();
		}
	}

	if (SettingWidgetClass)
	{
		SettingWidget = CreateWidget<UW_SettingWidget>(this, SettingWidgetClass);
		if (IsValid(SettingWidget))
		{
			SettingWidget->OnCloseClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingClosed);
			SettingWidget->AddToViewport(1);
			SettingWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AGameStartController::HandleGameStartRequested()
{
	if (USaveGISubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<USaveGISubsystem>())
	{
		SaveSubsystem->ResetSavedGame();
	}

	if (UMoneyGISubsystem* MoneySubsystem = GetGameInstance()->GetSubsystem<UMoneyGISubsystem>())
	{
		MoneySubsystem->InitMoney();
	}

	if (ULevelGISubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<ULevelGISubsystem>())
	{
		LevelSubsystem->SetBattleLevelIndex(0);
		LevelSubsystem->MoveShopLevel();
	}
}

void AGameStartController::HandleGameLoadRequested()
{
	if (USaveGISubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<USaveGISubsystem>())
	{
		if (SaveSubsystem->LoadCurrentGame())
		{
			if (ULevelGISubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<ULevelGISubsystem>())
			{
				LevelSubsystem->MoveLoadedShopLevel();
			}
		}
	}
}

void AGameStartController::HandleGameSettingRequested()
{
	if (IsValid(SettingWidget))
	{
		SettingWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameStartController::HandleGameTutorialRequested()
{
	if (ULevelGISubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<ULevelGISubsystem>())
	{
		LevelSubsystem->MovingTutorialLevel(0);
	}
}

void AGameStartController::HandleGameQuitRequested()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void AGameStartController::HandleSettingClosed()
{
	if (IsValid(SettingWidget))
	{
		SettingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
