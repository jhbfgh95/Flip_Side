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

void UW_GameStartWidget::NativeDestruct()
{
	if (IsValid(IntroMediaPlayer))
	{
		IntroMediaPlayer->OnMediaOpened.RemoveDynamic(this, &ThisClass::HandleIntroMediaOpened);
		IntroMediaPlayer->OnEndReached.RemoveDynamic(this, &ThisClass::HandleIntroMediaEndReached);
		IntroMediaPlayer->OnMediaOpenFailed.RemoveDynamic(this, &ThisClass::HandleIntroMediaOpenFailed);
		IntroMediaPlayer->OnSeekCompleted.RemoveDynamic(this, &ThisClass::HandleIntroSeekCompleted);
		IntroMediaPlayer->Pause();
	}
	Super::NativeDestruct();
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
		if (bShowLastFrameOnly)
		{
			SeekIntroLastFrame();
			return;
		}
		if (UCanvasPanelSlot* CanvasSlot = IsValid(IntroVideoImage) ? Cast<UCanvasPanelSlot>(IntroVideoImage->Slot) : nullptr)
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
	bShowLastFrameOnly = !bShouldPlayIntro;

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
		if (IsValid(IntroVideoImage))
		{
			IntroVideoImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IntroVideoImage->Slot))
			{
				CanvasSlot->SetZOrder(1);
			}
		}
		StartMenuBGM();
	}

	if (!IsValid(IntroMediaSource))
	{
		StartMenuBGM();
		return;
	}

	// 인트로 영상이 나오는 동안에는 이전 레벨에서 이어진 BGM이 겹치지 않게 줄여 둡니다.
	if (UWorld* World = GetWorld(); bShouldPlayIntro && IsValid(World))
	{
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->FadeOutLevelBGM();
		}
	}

	IntroMediaPlayer->OnMediaOpened.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaOpened);
	IntroMediaPlayer->OnEndReached.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaEndReached);
	IntroMediaPlayer->OnMediaOpenFailed.AddUniqueDynamic(this, &ThisClass::HandleIntroMediaOpenFailed);
	IntroMediaPlayer->OnSeekCompleted.AddUniqueDynamic(this, &ThisClass::HandleIntroSeekCompleted);
	// 복귀 시에는 처음부터 자동 재생되지 않도록 하고, 열기 완료 후 끝부분으로 이동합니다.
	IntroMediaPlayer->PlayOnOpen = false;
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

void UW_GameStartWidget::SeekIntroLastFrame()
{
	if (!IsValid(IntroMediaPlayer)) return;
	IntroMediaPlayer->Pause();
	const double DurationSeconds = IntroMediaPlayer->GetDuration().GetTotalSeconds();
	if (DurationSeconds <= 0.0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameStartIntro] Cannot seek: media duration is unavailable."));
		return;
	}
	const FTimespan LastFrameTime = FTimespan::FromSeconds(
		FMath::Max(0.0, DurationSeconds - FMath::Max(0.01, static_cast<double>(IntroLastFrameOffsetSeconds))));
	if (!IntroMediaPlayer->Seek(LastFrameTime))
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameStartIntro] Failed to seek to the last frame."));
	}
}

void UW_GameStartWidget::HandleIntroSeekCompleted()
{
	if (bShowLastFrameOnly && IsValid(IntroMediaPlayer))
	{
		IntroMediaPlayer->Pause();
	}
}

void UW_GameStartWidget::HandleIntroMediaEndReached()
{
	StartMenuBGM();
	if (!bShowLastFrameOnly)
	{
		bShowLastFrameOnly = true;
		SeekIntroLastFrame();
	}

	if (!IsValid(IntroVideoImage))
	{
		return;
	}
	IntroVideoImage->SetVisibility(ESlateVisibility::HitTestInvisible);

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
