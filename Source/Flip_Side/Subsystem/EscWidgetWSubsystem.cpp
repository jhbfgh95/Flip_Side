// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/EscWidgetWSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/FlipSideDevloperSettings.h"

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

	ESCWidgetInstance = CreateWidget<UUserWidget>(&World, ESCWidgetClass);
	if (IsValid(ESCWidgetInstance))
	{
		ESCWidgetInstance->AddToViewport(50);
		// 루트의 빈 영역은 아래 UI 입력을 통과시키고, 자식 버튼만 입력을 받습니다.
		ESCWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
