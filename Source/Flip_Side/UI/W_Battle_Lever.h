#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipSide_Enum.h"
#include "W_Battle_Lever.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBattlePhaseProgressRequested);

UCLASS()
class FLIP_SIDE_API UW_Battle_Lever : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetPhase(EPhaseState CurrentPhase);
	void PlayBossPhaseCompletionAnimation();

	FOnBattlePhaseProgressRequested OnPhaseProgressRequested;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PhaseProgressButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> LeverPull_Player;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> LeverPull_Boss;

	UFUNCTION()
	void HandlePhaseProgressButtonClicked();

	UFUNCTION()
	void HandlePlayerLeverAnimationFinished();

	UFUNCTION()
	void HandleBossLeverAnimationFinished();

	// 레버가 원래 자리로 올라오기 시작할 때 소리를 냅니다.
	UFUNCTION()
	void PlayLeverReturnSound();

	void UpdatePhaseProgressButtonState();

	EPhaseState DisplayedPhase = EPhaseState::None;
	bool bIsLeverAnimationPlaying = false;

	// 준비 단계는 내려갔다가 바로 올라오므로(PingPong), 올라오기 시작하는 순간을 이 타이머로 잡습니다.
	FTimerHandle LeverReturnSoundTimer;
};
