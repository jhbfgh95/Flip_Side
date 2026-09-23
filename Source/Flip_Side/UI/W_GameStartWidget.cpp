// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_GameStartWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/GameInstance.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Subsystem/SaveGISubsystem.h"

void UW_GameStartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameStartButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleGameStartButtonClicked);
	LoadButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleLoadButtonClicked);
	SettingButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingButtonClicked);
	TutorialButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleTutorialButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitButtonClicked);
}

void UW_GameStartWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateIntroVideoVisibility();
}

void UW_GameStartWidget::CollapseLoadButton()
{
	LoadButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_GameStartWidget::HandleGameStartButtonClicked()
{
	OnGameStartRequested.Broadcast();
}

void UW_GameStartWidget::HandleLoadButtonClicked()
{
	OnGameLoadRequested.Broadcast();
}

void UW_GameStartWidget::HandleSettingButtonClicked()
{
	OnGameSettingRequested.Broadcast();
}

void UW_GameStartWidget::HandleTutorialButtonClicked()
{
	OnGameTutorialRequested.Broadcast();
}

void UW_GameStartWidget::HandleQuitButtonClicked()
{
	OnGameQuitRequested.Broadcast();
}

void UW_GameStartWidget::HandleIntroMediaOpened(FString OpenedUrl)
{
	if (IsValid(IntroMediaPlayer))
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IntroVideoImage->Slot))
		{
			CanvasSlot->SetZOrder(20);
		}
		IntroMediaPlayer->Play();
	}
	else
	{
		HandleIntroMediaEndReached();
	}
}

void UW_GameStartWidget::UpdateIntroVideoVisibility()
{
	const USaveGISubsystem* SaveSubsystem = GetGameInstance()
		? GetGameInstance()->GetSubsystem<USaveGISubsystem>()
		: nullptr;
	const bool bShouldPlayIntro = !IsValid(SaveSubsystem) || !SaveSubsystem->HasReturnedToStartMenu();

	if (IsValid(IntroVideoImage))
	{
		IntroVideoImage->SetVisibility(ESlateVisibility::Visible);
	}

	if (!IsValid(IntroMediaPlayer))
	{
		return;
	}

	if (!bShouldPlayIntro)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IntroVideoImage->Slot))
		{
			CanvasSlot->SetZOrder(1);
		}
		IntroMediaPlayer->SetLooping(false);
		return;
	}

	if (!IsValid(IntroMediaSource))
	{
		return;
	}

	IntroMediaPlayer->OnMediaOpened.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaOpened);
	IntroMediaPlayer->OnEndReached.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaEndReached);
	IntroMediaPlayer->SetLooping(false);
	IntroMediaPlayer->Close();
	IntroMediaPlayer->OpenSource(IntroMediaSource);
}

void UW_GameStartWidget::HandleIntroMediaEndReached()
{
	if (!IsValid(IntroVideoImage))
	{
		return;
	}

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IntroVideoImage->Slot))
	{
		CanvasSlot->SetZOrder(1);
	}
}
