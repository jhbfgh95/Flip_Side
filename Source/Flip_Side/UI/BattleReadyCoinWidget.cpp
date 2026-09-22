// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattleReadyCoinWidget.h"

#include "Components/TextBlock.h"
#include "UI/ReadyCoinSlot.h"
#include "UI/BattleCoinInfoWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UBattleReadyCoinWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheReadyCoinSlots();
	UpdateReadyCoinCountText(0);
	if (IsValid(ToggleInfoButton))
		ToggleInfoButton->OnClicked.AddUniqueDynamic(this, &UBattleReadyCoinWidget::HandleToggleInfoClicked);
	SetInfoPageVisible(false);
}

void UBattleReadyCoinWidget::NativeDestruct()
{
	OnInfoSelectionReset.Broadcast();
	OnSlotHighlightClearRequested.Broadcast();
	if (IsValid(ToggleInfoButton))
		ToggleInfoButton->OnClicked.RemoveDynamic(this, &UBattleReadyCoinWidget::HandleToggleInfoClicked);
	for (UReadyCoinSlot* SlotWidget : ReadyCoinSlots)
	{
		if (!IsValid(SlotWidget)) continue;
		SlotWidget->OnReadyCoinSlotClicked.RemoveAll(this);
		SlotWidget->OnReadyCoinSlotHovered.RemoveAll(this);
		SlotWidget->OnReadyCoinSlotUnhovered.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UBattleReadyCoinWidget::ShowBattleCoinInfo(const FBattleCoinInfoViewData& InData)
{
	if (!IsValid(BattleCoinInfoWidget)) return;
	SetInfoPageVisible(true);
	BattleCoinInfoWidget->SetBattleCoinInfo(InData);
}

void UBattleReadyCoinWidget::ClearBattleCoinInfo()
{
	// 사망/수동 초기화는 내용만 비우고 사용자가 선택한 페이지는 유지합니다.
	if (IsValid(BattleCoinInfoWidget)) BattleCoinInfoWidget->ClearBattleCoinInfo();
}

void UBattleReadyCoinWidget::ToggleDescriptionDetails()
{
	if (bInfoPageVisible && IsValid(BattleCoinInfoWidget)) BattleCoinInfoWidget->ToggleDetailedDescriptions();
}

void UBattleReadyCoinWidget::HandleToggleInfoClicked()
{
	OnInfoSelectionReset.Broadcast();
	ClearBattleCoinInfo();
	SetInfoPageVisible(!bInfoPageVisible);
}

void UBattleReadyCoinWidget::SetInfoPageVisible(bool bVisible)
{
	if (bVisible) OnSlotHighlightClearRequested.Broadcast();
	bInfoPageVisible = bVisible;
	if (IsValid(ReadyInfoSwitcher) && ReadyInfoSwitcher->GetChildrenCount() >= 2)
		ReadyInfoSwitcher->SetActiveWidgetIndex(bVisible ? 1 : 0);
	if (IsValid(ReadyAreaTitleText)) ReadyAreaTitleText->SetText(bVisible
		? NSLOCTEXT("BattleReady", "InfoTitle", "배틀 코인 정보")
		: NSLOCTEXT("BattleReady", "ReadyTitle", "준비된 코인"));
	const ESlateVisibility CountVisibility = bVisible ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible;
	if (IsValid(CoinCountContainer)) CoinCountContainer->SetVisibility(CountVisibility);
	if (IsValid(ReadyCoinCountText)) ReadyCoinCountText->SetVisibility(CountVisibility);
	if (IsValid(TotalCoinCountText)) TotalCoinCountText->SetVisibility(CountVisibility);
}

void UBattleReadyCoinWidget::SetReadyCoins(const TArray<FBattleReadyCoinViewData>& InReadyCoins)
{
	int32 FilledSlotCount = 0;
	for (int32 SlotIndex = 0; SlotIndex < ReadyCoinSlots.Num(); ++SlotIndex)
	{
		UReadyCoinSlot* ReadyCoinSlot = ReadyCoinSlots[SlotIndex];
		if (!IsValid(ReadyCoinSlot))
		{
			continue;
		}

		if (InReadyCoins.IsValidIndex(SlotIndex) && InReadyCoins[SlotIndex].CoinInstanceID != INDEX_NONE)
		{
			ReadyCoinSlot->SetReadyCoinData(InReadyCoins[SlotIndex]);
			++FilledSlotCount;
		}
		else
		{
			ReadyCoinSlot->ClearReadyCoinData();
		}
	}

	UpdateReadyCoinCountText(FilledSlotCount);
}

void UBattleReadyCoinWidget::UpdateReadyCoinCountText(int32 FilledSlotCount)
{
	if (!IsValid(ReadyCoinCountText))
	{
		return;
	}

	const FLinearColor& CountColor = FilledSlotCount >= 7
		? ReadyCoinCountHighColor
		: (FilledSlotCount >= 3 ? ReadyCoinCountMediumColor : ReadyCoinCountLowColor);
	ReadyCoinCountText->SetText(FText::AsNumber(FilledSlotCount));
	ReadyCoinCountText->SetColorAndOpacity(FSlateColor(CountColor));
}

void UBattleReadyCoinWidget::CacheReadyCoinSlots()
{
	ReadyCoinSlots =
	{
		ReadyCoinSlot1,
		ReadyCoinSlot2,
		ReadyCoinSlot3,
		ReadyCoinSlot4,
		ReadyCoinSlot5,
		ReadyCoinSlot6,
		ReadyCoinSlot7,
		ReadyCoinSlot8,
		ReadyCoinSlot9,
		ReadyCoinSlot10
	};

	for (int32 SlotIndex = 0; SlotIndex < ReadyCoinSlots.Num(); ++SlotIndex)
	{
		UReadyCoinSlot* ReadyCoinSlot = ReadyCoinSlots[SlotIndex];
		if (!IsValid(ReadyCoinSlot))
		{
			UE_LOG(LogTemp, Warning, TEXT("[BattleReadyCoin] ReadyCoinSlot%d BindWidget is invalid."), SlotIndex + 1);
			continue;
		}

		ReadyCoinSlot->OnReadyCoinSlotClicked.RemoveAll(this);
		ReadyCoinSlot->OnReadyCoinSlotHovered.RemoveAll(this);
		ReadyCoinSlot->OnReadyCoinSlotUnhovered.RemoveAll(this);
		ReadyCoinSlot->OnReadyCoinSlotClicked.AddUObject(this, &UBattleReadyCoinWidget::HandleReadyCoinSlotClicked);
		ReadyCoinSlot->OnReadyCoinSlotHovered.AddUObject(this, &UBattleReadyCoinWidget::HandleReadyCoinSlotHovered);
		ReadyCoinSlot->OnReadyCoinSlotUnhovered.AddUObject(this, &UBattleReadyCoinWidget::HandleReadyCoinSlotUnhovered);
	}
}

void UBattleReadyCoinWidget::HandleReadyCoinSlotClicked(int32 CoinInstanceID)
{
	OnReadyCoinClicked.Broadcast(CoinInstanceID);
}

void UBattleReadyCoinWidget::HandleReadyCoinSlotHovered(int32 CoinInstanceID)
{
	if (!bInfoPageVisible && CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinHovered.Broadcast(CoinInstanceID);
	}
}

void UBattleReadyCoinWidget::HandleReadyCoinSlotUnhovered(int32 CoinInstanceID)
{
	if (CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinUnhovered.Broadcast(CoinInstanceID);
	}
}

