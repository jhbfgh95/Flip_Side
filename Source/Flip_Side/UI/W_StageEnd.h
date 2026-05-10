// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "W_StageEnd.generated.h"


UCLASS()
class FLIP_SIDE_API UW_StageEnd : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetStageEndFlag(int32 NewStageEndFlag);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStageEndFlagChanged(int32 NewStageEndFlag);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* StageClearPannel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* GameOverPannel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* GameClearPannel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* GotoShopButton;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* GotoMainLevel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* GameClearGotoMainLevel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* SpentWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentCoinCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentCoinMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentItemCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentItemMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentCardCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentCardMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* SpentTotalMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundCoinCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundCoinMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundItemCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundItemMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundCardCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundCardMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* RefundTotalMoneyText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CurrentMoneyText;

	UPROPERTY(BlueprintReadOnly, Category = "Stage End")
	int32 StageEndFlag = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stage End|Money")
	FStageMoneySummary DisplayMoneySummary;

	UPROPERTY(BlueprintReadOnly, Category = "Stage End|Money")
	int32 DisplayCurrentMoney = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void OnMoneySummaryChanged(const FStageMoneySummary& NewSummary, int32 CurrentMoney);

private:
	UFUNCTION()
	void StageClearedClicked();

	UFUNCTION()
	void GameOverClicked();

	UFUNCTION()
	void GameClearClicked();

	void UpdateMoneySummary();
	void SetMoneyText(class UTextBlock* TextBlock, int32 Money) const;
	void SetSpentWidgetVisibility(bool bVisible);
	void SetRefundTextVisibility(bool bVisible);
	void SetOptionalTextVisibility(class UTextBlock* TextBlock, bool bVisible) const;

};
