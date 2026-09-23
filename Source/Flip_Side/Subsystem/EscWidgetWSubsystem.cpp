// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/EscWidgetWSubsystem.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/SaveGISubsystem.h"
#include "UI/W_ESCWidget.h"

void UEscWidgetWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!InWorld.IsGameWorld())
	{
		return;
	}

	const FString LevelName = UGameplayStatics::GetCurrentLevelName(&InWorld, true);
	if (LevelName.Equals(TEXT("L_GameStart")))
	{
		return;
	}

	CreateAndShowESCWidget(InWorld);
}

void UEscWidgetWSubsystem::Deinitialize()
{
	if (IsValid(ESCWidgetInstance))
	{
		ESCWidgetInstance->OnQuitGameClicked.RemoveAll(this);
		ESCWidgetInstance->OnMainMenuClicked.RemoveAll(this);
		ESCWidgetInstance->RemoveFromParent();
		ESCWidgetInstance = nullptr;
	}

	Super::Deinitialize();
}

void UEscWidgetWSubsystem::CreateAndShowESCWidget(UWorld& World)
{
	if (IsValid(ESCWidgetInstance))
	{
		ESCWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}

	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	UClass* ESCWidgetClass = Settings ? Settings->SettingWidget.LoadSynchronous() : nullptr;
	if (!ESCWidgetClass)
	{
		return;
	}

	ESCWidgetInstance = CreateWidget<UW_ESCWidget>(&World, ESCWidgetClass);
	if (IsValid(ESCWidgetInstance))
	{
		ESCWidgetInstance->OnQuitGameClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitGameRequested);
		ESCWidgetInstance->OnMainMenuClicked.AddUniqueDynamic(this, &ThisClass::HandleMainMenuRequested);
		ESCWidgetInstance->AddToViewport(50);
		// 루트의 빈 영역은 아래 UI 입력을 통과시키고, 자식 버튼만 입력을 받습니다.
		ESCWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UEscWidgetWSubsystem::SaveCurrentGame() const
{
	UWorld* World = GetWorld();
	if (UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr)
	{
		if (USaveGISubsystem* SaveSubsystem = GameInstance->GetSubsystem<USaveGISubsystem>())
		{
			SaveSubsystem->SaveCurrentGame();
		}
	}
}

void UEscWidgetWSubsystem::HandleQuitGameRequested()
{
	SaveCurrentGame();
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}

void UEscWidgetWSubsystem::HandleMainMenuRequested()
{
	SaveCurrentGame();

	UWorld* World = GetWorld();
	if (UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr)
	{
		if (USaveGISubsystem* SaveSubsystem = GameInstance->GetSubsystem<USaveGISubsystem>())
		{
			SaveSubsystem->SetReturnedToStartMenu(true);
		}

		if (ULevelGISubsystem* LevelSubsystem = GameInstance->GetSubsystem<ULevelGISubsystem>())
		{
			LevelSubsystem->MoveStartLevel();
		}
	}
}
