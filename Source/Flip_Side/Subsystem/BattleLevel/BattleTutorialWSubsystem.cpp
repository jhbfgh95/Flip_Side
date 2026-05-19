// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleLevel/BattleTutorialWSubsystem.h"

#include "Actors/TutorialTargetPoint.h"
#include "BattleTutorialSequenceData.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayerController_FlipSide.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "UI/W_BattleTutorialOverlay.h"

namespace
{
	constexpr float BattleTutorialLeverAdvanceDelay = 3.1f;
}

bool UBattleTutorialWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->GetName().Contains(TEXT("L_Stage_Battle_Tutorial"));
}

void UBattleTutorialWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	const float InitDelay = Settings ? Settings->BattleTutorialInitDelay : 0.2f;

	InWorld.GetTimerManager().SetTimer(
		InitBattleTutorialTimerHandle,
		this,
		&UBattleTutorialWSubsystem::InitBattleTutorialFromSettings,
		InitDelay,
		false
	);
}

void UBattleTutorialWSubsystem::InitBattleTutorialFromSettings()
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings)
	{
		return;
	}

	UBattleTutorialSequenceData* LoadedSequenceData = Settings->BattleTutorialSequenceData.LoadSynchronous();
	TSubclassOf<UW_BattleTutorialOverlay> LoadedOverlayClass = Settings->BattleTutorialOverlayWidgetClass.LoadSynchronous();

	InitBattleTutorial(LoadedSequenceData, LoadedOverlayClass);
}

void UBattleTutorialWSubsystem::InitBattleTutorial(UBattleTutorialSequenceData* InSequenceData, TSubclassOf<UW_BattleTutorialOverlay> InOverlayClass, int32 ZOrder)
{
	if (!InSequenceData || !InOverlayClass)
	{
		return;
	}

	EndBattleTutorial();

	SequenceData = InSequenceData;
	CurrentStepIndex = 0;
	CurrentStepClickCount = 0;
	bInitialized = true;

	BattlePlayerController = Cast<ABattlePlayerController_FlipSide>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	CoinManager = GetWorld() ? GetWorld()->GetSubsystem<UCoinManagementWSubsystem>() : nullptr;
	BattleManager = GetWorld() ? GetWorld()->GetSubsystem<UBattleManagerWSubsystem>() : nullptr;

	if (BattlePlayerController)
	{
		OverlayWidget = CreateWidget<UW_BattleTutorialOverlay>(BattlePlayerController, InOverlayClass);
	}
	else
	{
		OverlayWidget = CreateWidget<UW_BattleTutorialOverlay>(GetWorld(), InOverlayClass);
	}

	if (!OverlayWidget)
	{
		EndBattleTutorial();
		return;
	}

	OverlayWidget->OnBattleTutorialOverlayClicked.AddUObject(this, &UBattleTutorialWSubsystem::HandleOverlayClicked);
	OverlayWidget->AddToViewport(ZOrder);

	CacheTutorialTargets();
	BindBattleEvents();
	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::EndBattleTutorial()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitBattleTutorialTimerHandle);
		World->GetTimerManager().ClearTimer(LeverAdvanceTimerHandle);
	}

	UnbindBattleEvents();

	if (OverlayWidget)
	{
		OverlayWidget->OnBattleTutorialOverlayClicked.RemoveAll(this);
		OverlayWidget->RemoveFromParent();
		OverlayWidget = nullptr;
	}

	SetTutorialInput(false);

	SequenceData = nullptr;
	CoinManager = nullptr;
	BattleManager = nullptr;
	BattlePlayerController = nullptr;
	TutorialTargetMap.Empty();
	CurrentStepIndex = INDEX_NONE;
	CurrentStepClickCount = 0;
	bInitialized = false;
}

void UBattleTutorialWSubsystem::AdvanceBattleTutorial()
{
	if (!bInitialized || !SequenceData)
	{
		return;
	}

	CurrentStepIndex++;
	CurrentStepClickCount = 0;

	if (!SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		EndBattleTutorial();
		return;
	}

	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::AdvanceAfterLeverAct()
{
	if (OverlayWidget)
	{
		OverlayWidget->SetVisibility(ESlateVisibility::Visible);
	}

	AdvanceBattleTutorial();
}

void UBattleTutorialWSubsystem::CacheTutorialTargets()
{
	TutorialTargetMap.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorialTargetPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ATutorialTargetPoint* TargetPoint = Cast<ATutorialTargetPoint>(Actor);
		if (!TargetPoint || TargetPoint->FocusId.IsNone())
		{
			continue;
		}

		TutorialTargetMap.Add(TargetPoint->FocusId, TargetPoint);
	}
}

void UBattleTutorialWSubsystem::BindBattleEvents()
{
	if (CoinManager)
	{
		CoinManager->OnBattleTutorialCoinSlotClicked.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinSlotClicked);
	}

	if (BattleManager)
	{
		BattleManager->OnBattleTutorialLeverTriggered.AddUObject(this, &UBattleTutorialWSubsystem::HandleLeverTriggered);
	}
}

void UBattleTutorialWSubsystem::UnbindBattleEvents()
{
	if (CoinManager)
	{
		CoinManager->OnBattleTutorialCoinSlotClicked.RemoveAll(this);
	}

	if (BattleManager)
	{
		BattleManager->OnBattleTutorialLeverTriggered.RemoveAll(this);
	}
}

void UBattleTutorialWSubsystem::ApplyCurrentStep()
{
	if (!SequenceData || !OverlayWidget || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		EndBattleTutorial();
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];

	OverlayWidget->SetTutorialText(CurrentStep.Text, CurrentStep.bUseTopTextBox);
	OverlayWidget->SetNextButtonEnabled(CurrentStep.AdvanceType == EBattleTutorialAdvanceType::OverlayClick ||
		CurrentStep.AdvanceType == EBattleTutorialAdvanceType::End);

	SetTutorialInput(CurrentStep.bUIOnly);

	if (CurrentStep.FocusId.IsNone())
	{
		OverlayWidget->SetFocusUV(FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);
	}
	else if (TObjectPtr<ATutorialTargetPoint>* TargetPoint = TutorialTargetMap.Find(CurrentStep.FocusId))
	{
		if (*TargetPoint)
		{
			OverlayWidget->SetFocusFromWorldLocation(BattlePlayerController, (*TargetPoint)->GetActorLocation(), GetHoleSizeForStep(CurrentStep));
		}
	}

	if (CurrentStep.AdvanceType == EBattleTutorialAdvanceType::End)
	{
		// End 단계는 마지막 안내를 보여준 뒤 Overlay 클릭으로 종료한다.
		OverlayWidget->SetNextButtonEnabled(true);
	}
}

void UBattleTutorialWSubsystem::HandleOverlayClicked()
{
	if (!SequenceData || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];
	if (CurrentStep.AdvanceType == EBattleTutorialAdvanceType::End)
	{
		EndBattleTutorial();
		return;
	}

	if (CurrentStep.AdvanceType != EBattleTutorialAdvanceType::OverlayClick)
	{
		return;
	}

	AdvanceBattleTutorial();
}

void UBattleTutorialWSubsystem::HandleCoinSlotClicked(ACoinActor* ClickedCoin)
{
	if (!SequenceData || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];
	if (CurrentStep.AdvanceType != EBattleTutorialAdvanceType::CoinSlotClick)
	{
		return;
	}

	CurrentStepClickCount++;
	if (CurrentStepClickCount >= CurrentStep.RequiredClickCount)
	{
		AdvanceBattleTutorial();
	}
}

void UBattleTutorialWSubsystem::HandleLeverTriggered()
{
	if (!SequenceData || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];
	if (CurrentStep.AdvanceType == EBattleTutorialAdvanceType::LeverClick)
	{
		if (OverlayWidget)
		{
			OverlayWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				LeverAdvanceTimerHandle,
				this,
				&UBattleTutorialWSubsystem::AdvanceAfterLeverAct,
				BattleTutorialLeverAdvanceDelay,
				false
			);
		}
		else
		{
			AdvanceAfterLeverAct();
		}
	}
}

void UBattleTutorialWSubsystem::SetTutorialInput(bool bUIOnly)
{
	if (BattlePlayerController)
	{
		BattlePlayerController->SetInputForTutorial(bUIOnly);
	}
}

FVector2D UBattleTutorialWSubsystem::GetHoleSizeForStep(const FBattleTutorialStep& Step) const
{
	if (!Step.HoleSize.IsNearlyZero())
	{
		return Step.HoleSize;
	}

	return GetDefaultHoleSize(Step.FocusId);
}

FVector2D UBattleTutorialWSubsystem::GetDefaultHoleSize(FName FocusId) const
{
	if (FocusId == TEXT("Lever"))
	{
		return FVector2D(0.06f, 0.14f);
	}

	if (FocusId == TEXT("CoinSlot"))
	{
		return FVector2D(0.2f, 0.15f);
	}

	if (FocusId == TEXT("Drawer"))
	{
		return FVector2D(0.3f, 0.25f);
	}

	if (FocusId == TEXT("ItemSlot"))
	{
		return FVector2D(0.08f, 0.1f);
	}

	if (FocusId == TEXT("Card"))
	{
		return FVector2D(0.3f, 1.0f);
	}

	return FVector2D(0.03f, 0.05f);
}
