// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattleReadyCoinWidget.h"

#include "Components/TextBlock.h"
#include "UI/ReadyCoinSlot.h"

void UBattleReadyCoinWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheReadyCoinSlots();
	UpdateReadyCoinCountText(0);
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
	if (CoinInstanceID != INDEX_NONE)
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

