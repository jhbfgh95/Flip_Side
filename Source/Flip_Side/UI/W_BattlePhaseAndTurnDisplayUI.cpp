// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BattlePhaseAndTurnDisplayUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
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
            return FText::FromString(TEXT("Coin Behavior"));
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

    if (IsValid(PhaseProgressButton))
    {
        PhaseProgressButton->OnClicked.RemoveAll(this);
        PhaseProgressButton->OnClicked.AddDynamic(this, &UW_BattlePhaseAndTurnDisplayUI::HandlePhaseProgressButtonClicked);
    }

	if (IsValid(LeverPull_Player))
	{
		FWidgetAnimationDynamicEvent PlayerAnimationFinishedEvent;
		PlayerAnimationFinishedEvent.BindDynamic(this, &UW_BattlePhaseAndTurnDisplayUI::HandlePlayerLeverAnimationFinished);
		BindToAnimationFinished(LeverPull_Player, PlayerAnimationFinishedEvent);
	}

	if (IsValid(LeverPull_Boss))
	{
		FWidgetAnimationDynamicEvent BossAnimationFinishedEvent;
		BossAnimationFinishedEvent.BindDynamic(this, &UW_BattlePhaseAndTurnDisplayUI::HandleBossLeverAnimationFinished);
		BindToAnimationFinished(LeverPull_Boss, BossAnimationFinishedEvent);
	}

	UpdatePhaseIndicator(EPhaseState::None);
}

void UW_BattlePhaseAndTurnDisplayUI::SetPhaseDisplay(EPhaseState CurrentPhase, int32 TurnCount)
{
	DisplayedPhase = CurrentPhase;
	UpdatePhaseIndicator(CurrentPhase);

	if (CurrentPhase == EPhaseState::SettingPhase)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	UpdatePhaseProgressButtonState();

    if (PhaseRichTextBlock)
    {
        PhaseRichTextBlock->SetText(GetPhaseText(CurrentPhase));
    }

    if (TurnCountTextBlock)
    {
        TurnCountTextBlock->SetText(FText::AsNumber(TurnCount));
    }

	SetVisibility(ESlateVisibility::Visible);
}

void UW_BattlePhaseAndTurnDisplayUI::HandlePhaseProgressButtonClicked()
{
	if (bIsLeverAnimationPlaying ||
		(DisplayedPhase != EPhaseState::CoinReadyPhase && DisplayedPhase != EPhaseState::CoinBehaviorPhase))
	{
		return;
	}

	if (!IsValid(LeverPull_Player))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePhaseUI] LeverPull_Player animation is not bound. Phase progress proceeds without animation."));
		OnPhaseProgressRequested.Broadcast();
		return;
	}

	bIsLeverAnimationPlaying = true;
	UpdatePhaseProgressButtonState();

	const EUMGSequencePlayMode::Type PlayMode = DisplayedPhase == EPhaseState::CoinReadyPhase
		? EUMGSequencePlayMode::PingPong
		: EUMGSequencePlayMode::Forward;
	PlayAnimation(LeverPull_Player, 0.f, 1, PlayMode);
}

void UW_BattlePhaseAndTurnDisplayUI::PlayBossPhaseCompletionAnimation()
{
	if (!IsValid(LeverPull_Boss))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePhaseUI] LeverPull_Boss animation is not bound."));
		return;
	}

	bIsLeverAnimationPlaying = true;
	UpdatePhaseProgressButtonState();
	PlayAnimation(LeverPull_Boss, 0.f, 1, EUMGSequencePlayMode::Forward);
}

void UW_BattlePhaseAndTurnDisplayUI::HandlePlayerLeverAnimationFinished()
{
	OnPhaseProgressRequested.Broadcast();
	bIsLeverAnimationPlaying = false;
	UpdatePhaseProgressButtonState();
}

void UW_BattlePhaseAndTurnDisplayUI::HandleBossLeverAnimationFinished()
{
	bIsLeverAnimationPlaying = false;
	UpdatePhaseProgressButtonState();
}

void UW_BattlePhaseAndTurnDisplayUI::UpdatePhaseProgressButtonState()
{
	if (!IsValid(PhaseProgressButton))
	{
		return;
	}

	const bool bCanProgressPhase =
		DisplayedPhase == EPhaseState::CoinReadyPhase ||
		DisplayedPhase == EPhaseState::CoinBehaviorPhase;
	PhaseProgressButton->SetIsEnabled(bCanProgressPhase && !bIsLeverAnimationPlaying);
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
