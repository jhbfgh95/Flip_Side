// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BattlePhaseAndTurnDisplayUI.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

namespace
{
    FText GetPhaseText(ETurnState CurrentTurn)
    {
        switch (CurrentTurn)
        {
        case ETurnState::CoinReadyTurn:
            return FText::FromString(TEXT("<CoinReady>Coin Ready</>"));
        case ETurnState::CoinSelectTurn:
            return FText::FromString(TEXT("<CoinSelect>Coin Select</>"));
        case ETurnState::BossTurn:
            return FText::FromString(TEXT("<Boss>Boss</>"));
        default:
            return FText::GetEmpty();
        }
    }
}

void UW_BattlePhaseAndTurnDisplayUI::SetTurnDisplay(ETurnState CurrentTurn, int32 TurnCount)
{
    if (PhaseRichTextBlock)
    {
        PhaseRichTextBlock->SetText(GetPhaseText(CurrentTurn));
    }

    if (TurnCountTextBlock)
    {
        TurnCountTextBlock->SetText(FText::AsNumber(TurnCount));
    }
}
