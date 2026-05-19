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
	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* PhaseRichTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TurnCountTextBlock;

public:
	void SetTurnDisplay(ETurnState CurrentTurn, int32 TurnCount);
};
