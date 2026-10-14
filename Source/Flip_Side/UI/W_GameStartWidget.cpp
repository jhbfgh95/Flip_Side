// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_GameStartWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
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
		StartMenuBGM();
		return;
	}

	if (!bShouldPlayIntro)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IntroVideoImage->Slot))
		{
			CanvasSlot->SetZOrder(1);
		}
		IntroMediaPlayer->SetLooping(false);
		StartMenuBGM();
		return;
	}

	if (!IsValid(IntroMediaSource))
	{
		StartMenuBGM();
		return;
	}

	// 인트로 영상이 나오는 동안에는 이전 레벨에서 이어진 BGM이 겹치지 않게 줄여 둡니다.
	if (UWorld* World = GetWorld())
	{
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->FadeOutLevelBGM();
		}
	}

	IntroMediaPlayer->OnMediaOpened.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaOpened);
	IntroMediaPlayer->OnEndReached.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaEndReached);
	IntroMediaPlayer->OnMediaOpenFailed.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaOpenFailed);
	IntroMediaPlayer->SetLooping(false);
	IntroMediaPlayer->Close();
	if (!IntroMediaPlayer->OpenSource(IntroMediaSource))
	{
		// 영상을 열지 못하면 영상 없이 바로 메뉴로 넘어가 BGM을 틉니다.
		HandleIntroMediaEndReached();
	}
}

void UW_GameStartWidget::HandleIntroMediaOpenFailed(FString FailedUrl)
{
	HandleIntroMediaEndReached();
}

void UW_GameStartWidget::HandleIntroMediaEndReached()
{
	StartMenuBGM();

	if (!IsValid(IntroVideoImage))
	{
		return;
	}

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IntroVideoImage->Slot))
	{
		CanvasSlot->SetZOrder(1);
	}
}

void UW_GameStartWidget::StartMenuBGM()
{
	if (UWorld* World = GetWorld())
	{
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->PlayLevelBGM();
		}
	}
}
