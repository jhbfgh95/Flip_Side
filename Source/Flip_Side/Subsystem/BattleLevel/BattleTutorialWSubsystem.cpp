// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleLevel/BattleTutorialWSubsystem.h"

#include "Actors/TutorialTargetPoint.h"
#include "BattleTutorialSequenceData.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayerController_FlipSide.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "UI/W_BattleTutorialOverlay.h"

bool UBattleTutorialWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->GetName().Contains(TEXT("L_Stage"));
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

	OverlayWidget->SetTutorialText(CurrentStep.Text);
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
			OverlayWidget->SetFocusFromWorldLocation(BattlePlayerController, (*TargetPoint)->GetActorLocation(), CurrentStep.HoleSize);
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
		AdvanceBattleTutorial();
	}
}

void UBattleTutorialWSubsystem::SetTutorialInput(bool bUIOnly)
{
	if (BattlePlayerController)
	{
		BattlePlayerController->SetInputForTutorial(bUIOnly);
	}
}
