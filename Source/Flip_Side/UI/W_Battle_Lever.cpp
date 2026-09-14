#include "UI/W_Battle_Lever.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"

void UW_Battle_Lever::NativeConstruct()
{
    Super::NativeConstruct();

    if (IsValid(PhaseProgressButton))
    {
        PhaseProgressButton->OnClicked.RemoveAll(this);
        PhaseProgressButton->OnClicked.AddDynamic(this, &UW_Battle_Lever::HandlePhaseProgressButtonClicked);
    }

	if (IsValid(LeverPull_Player))
	{
		UnbindAllFromAnimationFinished(LeverPull_Player);
		FWidgetAnimationDynamicEvent PlayerAnimationFinishedEvent;
		PlayerAnimationFinishedEvent.BindDynamic(this, &UW_Battle_Lever::HandlePlayerLeverAnimationFinished);
		BindToAnimationFinished(LeverPull_Player, PlayerAnimationFinishedEvent);
	}

	if (IsValid(LeverPull_Boss))
	{
		UnbindAllFromAnimationFinished(LeverPull_Boss);
		FWidgetAnimationDynamicEvent BossAnimationFinishedEvent;
		BossAnimationFinishedEvent.BindDynamic(this, &UW_Battle_Lever::HandleBossLeverAnimationFinished);
		BindToAnimationFinished(LeverPull_Boss, BossAnimationFinishedEvent);
	}

	UpdatePhaseProgressButtonState();
}

void UW_Battle_Lever::NativeDestruct()
{
    if (IsValid(PhaseProgressButton))
    {
        PhaseProgressButton->OnClicked.RemoveAll(this);
    }
    if (IsValid(LeverPull_Player))
    {
        UnbindAllFromAnimationFinished(LeverPull_Player);
    }
    if (IsValid(LeverPull_Boss))
    {
        UnbindAllFromAnimationFinished(LeverPull_Boss);
    }
    StopAllAnimations();
    bIsLeverAnimationPlaying = false;
    Super::NativeDestruct();
}

void UW_Battle_Lever::SetPhase(EPhaseState CurrentPhase)
{
    DisplayedPhase = CurrentPhase;
    UpdatePhaseProgressButtonState();
    SetVisibility(CurrentPhase == EPhaseState::SettingPhase
        ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void UW_Battle_Lever::HandlePhaseProgressButtonClicked()
{
	if (bIsLeverAnimationPlaying ||
		(DisplayedPhase != EPhaseState::CoinReadyPhase && DisplayedPhase != EPhaseState::CoinBehaviorPhase))
	{
		return;
	}

	if (!IsValid(LeverPull_Player))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleLever] LeverPull_Player animation is not bound. Phase progress proceeds without animation."));
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

void UW_Battle_Lever::PlayBossPhaseCompletionAnimation()
{
	if (!IsValid(LeverPull_Boss))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleLever] LeverPull_Boss animation is not bound."));
		return;
	}

	bIsLeverAnimationPlaying = true;
	UpdatePhaseProgressButtonState();
	PlayAnimation(LeverPull_Boss, 0.f, 1, EUMGSequencePlayMode::Forward);
}

void UW_Battle_Lever::HandlePlayerLeverAnimationFinished()
{
	OnPhaseProgressRequested.Broadcast();
	bIsLeverAnimationPlaying = false;
	UpdatePhaseProgressButtonState();
}

void UW_Battle_Lever::HandleBossLeverAnimationFinished()
{
	bIsLeverAnimationPlaying = false;
	UpdatePhaseProgressButtonState();
}

void UW_Battle_Lever::UpdatePhaseProgressButtonState()
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

