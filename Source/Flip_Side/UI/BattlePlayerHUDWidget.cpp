// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattlePlayerHUDWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "UI/BattleCoinSlotWidget.h"
#include "UI/BattleCardSlotWidget.h"
#include "UI/BattleItemSlotWidget.h"
#include "UI/BattleReadyCoinWidget.h"
#include "UI/W_CoinSlotInfo.h"
#include "UI/W_ItemInfo.h"
#include "UI/W_CardWidget.h"
#include "UI/W_BossHP.h"
#include "UI/BattleBossPatternHUDWidget.h"
#include "UI/W_BattlePhaseAndTurnDisplayUI.h"

void UBattlePlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(BattleReadyCoinWidget))
	{
		BattleReadyCoinWidget->OnReadyCoinClicked.AddUObject(this, &UBattlePlayerHUDWidget::HandleReadyCoinClicked);
	}

	if (IsValid(PhaseAndTurnDisplayWidget))
	{
		PhaseAndTurnDisplayWidget->OnPhaseProgressRequested.RemoveAll(this);
		PhaseAndTurnDisplayWidget->OnPhaseProgressRequested.AddUObject(this, &UBattlePlayerHUDWidget::HandlePhaseProgressRequested);
	}

	CacheFixedItemSlots();
	CacheFixedCardSlots();
}

void UBattlePlayerHUDWidget::SetCoinSlots(const TArray<FBattleCoinSlotViewData>& InCoinSlots)
{
	CoinSlotViewDataByNumber.Reset();
	EnsureCoinSlotWidgets(InCoinSlots.Num());

	for (int32 SlotIndex = 0; SlotIndex < CoinSlotWidgets.Num(); ++SlotIndex)
	{
		UBattleCoinSlotWidget* CoinSlotWidget = CoinSlotWidgets[SlotIndex];
		if (!IsValid(CoinSlotWidget))
		{
			continue;
		}

		if (InCoinSlots.IsValidIndex(SlotIndex))
		{
			CoinSlotWidget->SetSlotData(InCoinSlots[SlotIndex]);
			CoinSlotViewDataByNumber.Add(InCoinSlots[SlotIndex].SlotNumber, InCoinSlots[SlotIndex]);
		}
		else
		{
			CoinSlotWidget->ClearSlotData();
		}
	}
}

void UBattlePlayerHUDWidget::SetReadyCoins(const TArray<FBattleReadyCoinViewData>& InReadyCoins)
{
	if (IsValid(BattleReadyCoinWidget))
	{
		BattleReadyCoinWidget->SetReadyCoins(InReadyCoins);
	}
}

void UBattlePlayerHUDWidget::SetBossHUDData(const FBossHUDData& InData)
{
	if (IsValid(BossHPWidget))
	{
		BossHPWidget->SetBossHUDData(InData);
	}

	if (IsValid(BossPatternWidget))
	{
		BossPatternWidget->SetBossHUDData(InData);
	}
}

void UBattlePlayerHUDWidget::SetItemSlots(const TArray<FBattleItemSlotViewData>& InItemSlots)
{
	ItemSlotViewDataByID.Reset();

	for (int32 SlotIndex = 0; SlotIndex < ItemSlotWidgets.Num(); ++SlotIndex)
	{
		UBattleItemSlotWidget* ItemSlotWidget = ItemSlotWidgets[SlotIndex];
		if (!IsValid(ItemSlotWidget))
		{
			continue;
		}

		if (InItemSlots.IsValidIndex(SlotIndex))
		{
			const FBattleItemSlotViewData& ItemSlotData = InItemSlots[SlotIndex];
			ItemSlotWidget->SetItemData(ItemSlotData);
			ItemSlotViewDataByID.Add(ItemSlotData.ItemData.ItemID, ItemSlotData);
		}
		else
		{
			ItemSlotWidget->ClearItemData();
		}
	}
}

void UBattlePlayerHUDWidget::SetCardSlots(const TArray<FBattleCardSlotViewData>& InCardSlots)
{
	CardSlotViewDataByNumber.Reset();

	for (int32 SlotIndex = 0; SlotIndex < CardSlotWidgets.Num(); ++SlotIndex)
	{
		UBattleCardSlotWidget* CardSlotWidget = CardSlotWidgets[SlotIndex];
		if (!IsValid(CardSlotWidget))
		{
			continue;
		}

		if (InCardSlots.IsValidIndex(SlotIndex))
		{
			const FBattleCardSlotViewData& CardSlotData = InCardSlots[SlotIndex];
			CardSlotWidget->SetCardData(CardSlotData);
			CardSlotViewDataByNumber.Add(CardSlotData.SlotNumber, CardSlotData);
		}
		else
		{
			CardSlotWidget->ClearCardData();
		}
	}
}

void UBattlePlayerHUDWidget::SetPhaseDisplay(EPhaseState CurrentPhase, int32 TurnCount)
{
	if (IsValid(PhaseAndTurnDisplayWidget))
	{
		PhaseAndTurnDisplayWidget->SetPhaseDisplay(CurrentPhase, TurnCount);
	}
}

void UBattlePlayerHUDWidget::PlayBossPhaseCompletionAnimation()
{
	if (IsValid(PhaseAndTurnDisplayWidget))
	{
		PhaseAndTurnDisplayWidget->PlayBossPhaseCompletionAnimation();
	}
}

void UBattlePlayerHUDWidget::EnsureCoinSlotWidgets(int32 RequiredCount)
{
	if (!IsValid(CoinSlotContainer) || !BattleCoinSlotWidgetClass)
	{
		return;
	}

	while (CoinSlotWidgets.Num() < RequiredCount)
	{
		UBattleCoinSlotWidget* CoinSlotWidget = CreateWidget<UBattleCoinSlotWidget>(this, BattleCoinSlotWidgetClass);
		if (!IsValid(CoinSlotWidget))
		{
			return;
		}

		CoinSlotWidget->OnBattleCoinSlotClicked.AddUObject(this, &UBattlePlayerHUDWidget::HandleCoinSlotClicked);
		CoinSlotWidget->OnBattleCoinSlotHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleCoinSlotHovered);
		CoinSlotWidget->OnBattleCoinSlotUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleCoinSlotUnhovered);
		CoinSlotContainer->AddChild(CoinSlotWidget);
		CoinSlotWidgets.Add(CoinSlotWidget);
	}
}

void UBattlePlayerHUDWidget::CacheFixedItemSlots()
{
	ItemSlotWidgets = { ItemSlot1, ItemSlot2, ItemSlot3 };
	for (UBattleItemSlotWidget* ItemSlotWidget : ItemSlotWidgets)
	{
		if (!IsValid(ItemSlotWidget))
		{
			continue;
		}

		ItemSlotWidget->OnBattleItemSlotClicked.RemoveAll(this);
		ItemSlotWidget->OnBattleItemSlotHovered.RemoveAll(this);
		ItemSlotWidget->OnBattleItemSlotUnhovered.RemoveAll(this);
		ItemSlotWidget->OnBattleItemSlotClicked.AddUObject(this, &UBattlePlayerHUDWidget::HandleItemSlotClicked);
		ItemSlotWidget->OnBattleItemSlotHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleItemSlotHovered);
		ItemSlotWidget->OnBattleItemSlotUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleItemSlotUnhovered);
	}
}

void UBattlePlayerHUDWidget::CacheFixedCardSlots()
{
	CardSlotWidgets = { CardSlot1, CardSlot2, CardSlot3 };
	for (UBattleCardSlotWidget* CardSlotWidget : CardSlotWidgets)
	{
		if (!IsValid(CardSlotWidget))
		{
			continue;
		}

		CardSlotWidget->OnBattleCardSlotHovered.RemoveAll(this);
		CardSlotWidget->OnBattleCardSlotUnhovered.RemoveAll(this);
		CardSlotWidget->OnBattleCardSlotHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleCardSlotHovered);
		CardSlotWidget->OnBattleCardSlotUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleCardSlotUnhovered);
	}
}

void UBattlePlayerHUDWidget::HandleCoinSlotClicked(int32 SlotNumber)
{
	OnCoinSlotClicked.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleCoinSlotHovered(int32 SlotNumber)
{
	const FBattleCoinSlotViewData* CoinSlotData = CoinSlotViewDataByNumber.Find(SlotNumber);
	if (CoinSlotData && IsValid(PopupLayer) && CoinSlotInfoWidgetClass)
	{
		if (!IsValid(CoinSlotInfoWidget))
		{
			CoinSlotInfoWidget = CreateWidget<UW_CoinSlotInfo>(this, CoinSlotInfoWidgetClass);
			if (IsValid(CoinSlotInfoWidget))
			{
				PopupLayer->AddChild(CoinSlotInfoWidget);
			}
		}

		if (IsValid(CoinSlotInfoWidget))
		{
			CoinSlotInfoWidget->SetCoinSlotInfo(*CoinSlotData);
			CoinSlotInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			ApplyPopupAnchorLayout(CoinSlotInfoWidget, CoinSlotPopupAnchor);
		}
	}

	OnCoinSlotHovered.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleCoinSlotUnhovered(int32 SlotNumber)
{
	if (IsValid(CoinSlotInfoWidget))
	{
		CoinSlotInfoWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	OnCoinSlotUnhovered.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleReadyCoinClicked(int32 CoinInstanceID)
{
	OnReadyCoinClicked.Broadcast(CoinInstanceID);
}

void UBattlePlayerHUDWidget::HandlePhaseProgressRequested()
{
	OnPhaseProgressClicked.Broadcast();
}

void UBattlePlayerHUDWidget::HandleItemSlotClicked(int32 ItemID)
{
	OnItemSlotClicked.Broadcast(ItemID);
}

void UBattlePlayerHUDWidget::HandleItemSlotHovered(int32 ItemID)
{
	const FBattleItemSlotViewData* ItemSlotData = ItemSlotViewDataByID.Find(ItemID);
	if (!ItemSlotData || !IsValid(PopupLayer) || !ItemInfoWidgetClass)
	{
		return;
	}

	if (!IsValid(ItemInfoWidget))
	{
		ItemInfoWidget = CreateWidget<UW_ItemInfo>(this, ItemInfoWidgetClass);
		if (IsValid(ItemInfoWidget))
		{
			PopupLayer->AddChild(ItemInfoWidget);
		}
	}

	if (IsValid(ItemInfoWidget))
	{
		ItemInfoWidget->UpdateItemInfo(ItemSlotData->ItemData);
		ItemInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		ApplyPopupAnchorLayout(ItemInfoWidget, ItemPopupAnchor);
	}
}

void UBattlePlayerHUDWidget::HandleItemSlotUnhovered(int32 ItemID)
{
	if (IsValid(ItemInfoWidget))
	{
		ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBattlePlayerHUDWidget::HandleCardSlotHovered(int32 SlotNumber)
{
	const FBattleCardSlotViewData* CardSlotData = CardSlotViewDataByNumber.Find(SlotNumber);
	if (!CardSlotData || !CardSlotData->bOccupied || !IsValid(PopupLayer) || !CardInfoWidgetClass)
	{
		return;
	}

	if (!IsValid(CardInfoWidget))
	{
		CardInfoWidget = CreateWidget<UW_CardWidget>(this, CardInfoWidgetClass);
		if (IsValid(CardInfoWidget))
		{
			PopupLayer->AddChild(CardInfoWidget);
		}
	}

	if (IsValid(CardInfoWidget))
	{
		CardInfoWidget->InitCard(CardSlotData->CardData);
		CardInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		ApplyPopupAnchorLayout(CardInfoWidget, CardPopupAnchor);
	}
}

void UBattlePlayerHUDWidget::HandleCardSlotUnhovered(int32 SlotNumber)
{
	if (IsValid(CardInfoWidget))
	{
		CardInfoWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBattlePlayerHUDWidget::ApplyPopupAnchorLayout(UUserWidget* PopupWidget, UWidget* PopupAnchor)
{
	if (!IsValid(PopupWidget) || !IsValid(PopupAnchor))
	{
		return;
	}

	UCanvasPanelSlot* PopupCanvasSlot = Cast<UCanvasPanelSlot>(PopupWidget->Slot);
	UCanvasPanelSlot* AnchorCanvasSlot = Cast<UCanvasPanelSlot>(PopupAnchor->Slot);
	if (!IsValid(PopupCanvasSlot) || !IsValid(AnchorCanvasSlot))
	{
		return;
	}

	PopupCanvasSlot->SetAnchors(AnchorCanvasSlot->GetAnchors());
	PopupCanvasSlot->SetAlignment(AnchorCanvasSlot->GetAlignment());
	PopupCanvasSlot->SetPosition(AnchorCanvasSlot->GetPosition());
	PopupCanvasSlot->SetSize(AnchorCanvasSlot->GetSize());
	PopupCanvasSlot->SetAutoSize(AnchorCanvasSlot->GetAutoSize());
}

