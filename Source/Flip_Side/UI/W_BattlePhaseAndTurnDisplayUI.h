// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipSide_Enum.h"
#include "W_BattlePhaseAndTurnDisplayUI.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_BattlePhaseAndTurnDisplayUI : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidgetOptional))
	class URichTextBlock* PhaseRichTextBlock;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* TurnCountTextBlock;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* CoinReadyIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* CoinActionIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* BossPhaseIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* CoinReadyArrow;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* CoinActArrow;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* BossArrow;

public:
	virtual void NativeConstruct() override;

	void SetPhaseDisplay(EPhaseState CurrentPhase, int32 TurnCount);

private:
	void UpdatePhaseIndicator(EPhaseState CurrentPhase);
	void SetPhaseIconColor(class UImage* PhaseIcon, const FLinearColor& IconColor);
	void SetPhaseArrowVisibility(class UWidget* PhaseArrow, bool bVisible);

};
