// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopTutorialWSubsystem.h"


bool UShopTutorialWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
        return false;

    UWorld* World = Cast<UWorld>(Outer);
    if (!World)
        return false;

    const FString MapName = World->GetName();
    return MapName.Contains(TEXT("L_Tutorial_Shop_Level")) ||
        MapName.Contains(TEXT("L_Tutorial_TutoShop_Level"));
}

void UShopTutorialWSubsystem::SetTutorialActionSequence(const TArray<EShopTutorialAction>& InActionSequence)
{
	TutorialActionSequence = InActionSequence;
	CurrentStepIndex = INDEX_NONE;
}

void UShopTutorialWSubsystem::StartTutorial()
{
	CurrentStepIndex = TutorialActionSequence.IsEmpty() ? INDEX_NONE : 0;
	if (IsTutorialActive())
	{
		OnTutorialStarted.Broadcast(CurrentStepIndex);
	}
}

bool UShopTutorialWSubsystem::ReportAction(EShopTutorialAction Action)
{
	if (!IsTutorialActive() || GetExpectedAction() != Action)
	{
		return false;
	}

	++CurrentStepIndex;
	const EShopTutorialAction NextAction = GetExpectedAction();
	OnTutorialStepChanged.Broadcast(CurrentStepIndex, Action, NextAction);

	if (CurrentStepIndex >= TutorialActionSequence.Num())
	{
		OnTutorialCompleted.Broadcast();
	}

	return true;
}

EShopTutorialAction UShopTutorialWSubsystem::GetExpectedAction() const
{
	return IsTutorialActive()
		? TutorialActionSequence[CurrentStepIndex]
		: EShopTutorialAction::None;
}

bool UShopTutorialWSubsystem::IsTutorialActive() const
{
	return TutorialActionSequence.IsValidIndex(CurrentStepIndex);
}
