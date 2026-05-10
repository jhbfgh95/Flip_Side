#include "UI/W_StageEnd.h"
#include "LevelGISubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void UW_StageEnd::NativeConstruct()
{
    Super::NativeConstruct();

    if (GotoShopButton)
    {
        GotoShopButton->OnClicked.AddDynamic(this, &UW_StageEnd::StageClearedClicked);
    }

    if (GotoMainLevel)
    {
        GotoMainLevel->OnClicked.AddDynamic(this, &UW_StageEnd::GameOverClicked);
    }

    if (GameClearGotoMainLevel)
    {
        GameClearGotoMainLevel->OnClicked.AddDynamic(this, &UW_StageEnd::GameClearClicked);
    }

    UpdateMoneySummary();
}

void UW_StageEnd::SetStageEndFlag(int32 NewStageEndFlag)
{
    StageEndFlag = NewStageEndFlag;
    if (StageClearPannel)
    {
        StageClearPannel->SetVisibility(StageEndFlag == 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }

    if (GameOverPannel)
    {
        GameOverPannel->SetVisibility(StageEndFlag == 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }

    if (GameClearPannel)
    {
        GameClearPannel->SetVisibility(StageEndFlag == 2 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }

    OnStageEndFlagChanged(StageEndFlag);
    UpdateMoneySummary();
}

void UW_StageEnd::UpdateMoneySummary()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UMoneyGISubsystem* MoneyManager = GameInstance ? GameInstance->GetSubsystem<UMoneyGISubsystem>() : nullptr;
    if (!MoneyManager)
    {
        return;
    }

    switch (StageEndFlag)
    {
    case 0: // StageClear: 이번 스테이지 사용/청산 요약
        DisplayMoneySummary = MoneyManager->GetLastStageMoneySummary();
        break;
    case 1: // GameOver: 이번 스테이지 사용 요약
        DisplayMoneySummary = MoneyManager->GetLastStageMoneySummary();
        break;
    case 2: // GameClear: 한 판 전체 사용 요약
        DisplayMoneySummary = MoneyManager->GetRunMoneySummary();
        break;
    default:
        DisplayMoneySummary = FStageMoneySummary();
        break;
    }
    DisplayCurrentMoney = MoneyManager->GetCurrentMoney();

    SetMoneyText(SpentCoinCountText, DisplayMoneySummary.SpentCoinCount);
    SetMoneyText(SpentCoinMoneyText, DisplayMoneySummary.SpentCoin);
    SetMoneyText(SpentItemCountText, DisplayMoneySummary.SpentItemCount);
    SetMoneyText(SpentItemMoneyText, DisplayMoneySummary.SpentItem);
    SetMoneyText(SpentCardCountText, DisplayMoneySummary.SpentCardCount);
    SetMoneyText(SpentCardMoneyText, DisplayMoneySummary.SpentCard);
    SetMoneyText(SpentTotalMoneyText, DisplayMoneySummary.TotalSpent);

    SetMoneyText(RefundCoinCountText, DisplayMoneySummary.RefundCoinCount);
    SetMoneyText(RefundCoinMoneyText, DisplayMoneySummary.RefundCoin);
    SetMoneyText(RefundItemCountText, DisplayMoneySummary.RefundItemCount);
    SetMoneyText(RefundItemMoneyText, DisplayMoneySummary.RefundItem);
    SetMoneyText(RefundCardCountText, DisplayMoneySummary.RefundCardCount);
    SetMoneyText(RefundCardMoneyText, DisplayMoneySummary.RefundCard);
    SetMoneyText(RefundTotalMoneyText, DisplayMoneySummary.TotalRefund);
    SetMoneyText(CurrentMoneyText, DisplayCurrentMoney);
    SetSpentWidgetVisibility(StageEndFlag == 1 || StageEndFlag == 2);
    SetRefundTextVisibility(StageEndFlag == 0);

    OnMoneySummaryChanged(DisplayMoneySummary, DisplayCurrentMoney);
}

void UW_StageEnd::SetMoneyText(UTextBlock* TextBlock, int32 Money) const
{
    if (!TextBlock)
    {
        return;
    }

    TextBlock->SetText(FText::AsNumber(Money));
}

void UW_StageEnd::SetSpentWidgetVisibility(bool bVisible)
{
    if (!SpentWidget)
    {
        return;
    }

    SpentWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UW_StageEnd::SetRefundTextVisibility(bool bVisible)
{
    SetOptionalTextVisibility(RefundCoinCountText, bVisible);
    SetOptionalTextVisibility(RefundCoinMoneyText, bVisible);
    SetOptionalTextVisibility(RefundItemCountText, bVisible);
    SetOptionalTextVisibility(RefundItemMoneyText, bVisible);
    SetOptionalTextVisibility(RefundCardCountText, bVisible);
    SetOptionalTextVisibility(RefundCardMoneyText, bVisible);
    SetOptionalTextVisibility(RefundTotalMoneyText, bVisible);
}

void UW_StageEnd::SetOptionalTextVisibility(UTextBlock* TextBlock, bool bVisible) const
{
    if (!TextBlock)
    {
        return;
    }

    TextBlock->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UW_StageEnd::StageClearedClicked()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    ULevelGISubsystem* LevelMan = GameInstance ? GameInstance->GetSubsystem<ULevelGISubsystem>() : nullptr;
    if (!LevelMan) return;

    if (LevelMan->GetBattleLevelIndex() == 0)
    {
        LevelMan->MoveStartLevel();
        return;
    }

    if (UMoneyGISubsystem* MoneyManager = GameInstance->GetSubsystem<UMoneyGISubsystem>())
    {
        MoneyManager->ResetStageMoneySummary();
    }

    LevelMan->MoveShopLevel();
}

void UW_StageEnd::GameOverClicked()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    ULevelGISubsystem* LevelMan = GameInstance ? GameInstance->GetSubsystem<ULevelGISubsystem>() : nullptr;
    if (!LevelMan) return;

    LevelMan->MoveStartLevel();
}

void UW_StageEnd::GameClearClicked()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    ULevelGISubsystem* LevelMan = GameInstance ? GameInstance->GetSubsystem<ULevelGISubsystem>() : nullptr;
    if (!LevelMan) return;

    LevelMan->MoveStartLevel();
}
