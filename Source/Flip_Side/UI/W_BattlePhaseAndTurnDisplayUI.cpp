// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BattlePhaseAndTurnDisplayUI.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

namespace
{
	const FLinearColor InactiveIconColor(1.f, 1.f, 1.f, 1.f); // FFFFFFFF
	const FLinearColor ReadyPhaseIconColor = FLinearColor(FColor::FromHex(TEXT("54FF00FF")));
	const FLinearColor BehaviorPhaseIconColor = FLinearColor(FColor::FromHex(TEXT("FFC600FF")));
	const FLinearColor BossPhaseIconColor = FLinearColor(FColor::FromHex(TEXT("FF3700FF")));

    FText GetPhaseText(EPhaseState CurrentPhase)
    {
        switch (CurrentPhase)
        {
        case EPhaseState::CoinReadyPhase:
            return FText::FromString(TEXT("<CoinReady>Coin Ready</>"));
        case EPhaseState::CoinBehaviorPhase:
            return FText::FromString(TEXT("<CoinBehavior>Coin Behavior</>"));
        case EPhaseState::BossPhase:
            return FText::FromString(TEXT("<Boss>Boss</>"));
        default:
            return FText::GetEmpty();
        }
    }
}

void UW_BattlePhaseAndTurnDisplayUI::NativeConstruct()
{
    Super::NativeConstruct();
    UpdatePhaseIndicator(EPhaseState::None);
}

void UW_BattlePhaseAndTurnDisplayUI::SetPhaseDisplay(EPhaseState CurrentPhase, int32 TurnCount)
{
	UpdatePhaseIndicator(CurrentPhase);

	if (CurrentPhase == EPhaseState::SettingPhase)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

    if (IsValid(PhaseRichTextBlock))
    {
        PhaseRichTextBlock->SetText(GetPhaseText(CurrentPhase));
    }

    if (IsValid(TurnCountTextBlock))
    {
        TurnCountTextBlock->SetText(FText::AsNumber(TurnCount));
    }

	SetVisibility(ESlateVisibility::Visible);
}

void UW_BattlePhaseAndTurnDisplayUI::UpdatePhaseIndicator(EPhaseState CurrentPhase)
{
	SetPhaseIconColor(
		CoinReadyIcon,
		CurrentPhase == EPhaseState::CoinReadyPhase ? ReadyPhaseIconColor : InactiveIconColor
	);
	SetPhaseIconColor(
		CoinActionIcon,
		CurrentPhase == EPhaseState::CoinBehaviorPhase ? BehaviorPhaseIconColor : InactiveIconColor
	);
	SetPhaseIconColor(
		BossPhaseIcon,
		CurrentPhase == EPhaseState::BossPhase ? BossPhaseIconColor : InactiveIconColor
	);

	SetPhaseArrowVisibility(CoinReadyArrow, CurrentPhase == EPhaseState::CoinReadyPhase);
	SetPhaseArrowVisibility(CoinActArrow, CurrentPhase == EPhaseState::CoinBehaviorPhase);
	SetPhaseArrowVisibility(BossArrow, CurrentPhase == EPhaseState::BossPhase);
}

void UW_BattlePhaseAndTurnDisplayUI::SetPhaseIconColor(
	UImage* PhaseIcon,
	const FLinearColor& IconColor)
{
	if (!IsValid(PhaseIcon))
	{
		return;
	}

	PhaseIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	PhaseIcon->SetColorAndOpacity(IconColor);
}

void UW_BattlePhaseAndTurnDisplayUI::SetPhaseArrowVisibility(UWidget* PhaseArrow, bool bVisible)
{
	if (IsValid(PhaseArrow))
	{
		PhaseArrow->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}
