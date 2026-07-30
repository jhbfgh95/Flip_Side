// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattlePlayerHUDWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "UI/BattleCoinSlotWidget.h"
#include "UI/BattleCardSlotWidget.h"
#include "UI/BattleItemSlotWidget.h"
#include "UI/BattleReadyCoinWidget.h"
#include "UI/W_ItemInfo.h"
#include "UI/W_CardWidget.h"
#include "UI/W_BossHP.h"

void UBattlePlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(BattleReadyCoinWidget))
	{
		BattleReadyCoinWidget->OnReadyCoinClicked.AddUObject(this, &UBattlePlayerHUDWidget::HandleReadyCoinClicked);
	}
}

void UBattlePlayerHUDWidget::SetCoinSlots(const TArray<FBattleCoinSlotViewData>& InCoinSlots)
{
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
}

void UBattlePlayerHUDWidget::SetItemSlots(const TArray<FBattleItemSlotViewData>& InItemSlots)
{
	EnsureItemSlotWidgets(InItemSlots.Num());
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
	EnsureCardSlotWidgets(InCardSlots.Num());
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

void UBattlePlayerHUDWidget::EnsureItemSlotWidgets(int32 RequiredCount)
{
	if (!IsValid(ItemSlotContainer) || !BattleItemSlotWidgetClass)
	{
		return;
	}

	while (ItemSlotWidgets.Num() < RequiredCount)
	{
		UBattleItemSlotWidget* ItemSlotWidget = CreateWidget<UBattleItemSlotWidget>(this, BattleItemSlotWidgetClass);
		if (!IsValid(ItemSlotWidget))
		{
			return;
		}

		ItemSlotWidget->OnBattleItemSlotClicked.AddUObject(this, &UBattlePlayerHUDWidget::HandleItemSlotClicked);
		ItemSlotWidget->OnBattleItemSlotHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleItemSlotHovered);
		ItemSlotWidget->OnBattleItemSlotUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleItemSlotUnhovered);
		ItemSlotContainer->AddChild(ItemSlotWidget);
		ItemSlotWidgets.Add(ItemSlotWidget);
	}
}

void UBattlePlayerHUDWidget::EnsureCardSlotWidgets(int32 RequiredCount)
{
	if (!IsValid(CardSlotContainer) || !BattleCardSlotWidgetClass)
	{
		return;
	}

	while (CardSlotWidgets.Num() < RequiredCount)
	{
		UBattleCardSlotWidget* CardSlotWidget = CreateWidget<UBattleCardSlotWidget>(this, BattleCardSlotWidgetClass);
		if (!IsValid(CardSlotWidget))
		{
			return;
		}

		CardSlotWidget->OnBattleCardSlotHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleCardSlotHovered);
		CardSlotWidget->OnBattleCardSlotUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleCardSlotUnhovered);
		CardSlotContainer->AddChild(CardSlotWidget);
		CardSlotWidgets.Add(CardSlotWidget);
	}
}

void UBattlePlayerHUDWidget::HandleCoinSlotClicked(int32 SlotNumber)
{
	OnCoinSlotClicked.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleCoinSlotHovered(int32 SlotNumber)
{
	// TODO: CoinSlotInfoWidget이 준비되면 HUD PopupLayer에서 생성합니다.
	OnCoinSlotHovered.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleCoinSlotUnhovered(int32 SlotNumber)
{
	// TODO: CoinSlotInfoWidget이 준비되면 HUD PopupLayer의 팝업을 제거합니다.
	OnCoinSlotUnhovered.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleReadyCoinClicked(int32 CoinInstanceID)
{
	OnReadyCoinClicked.Broadcast(CoinInstanceID);
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
		PositionPopupAtCursor(ItemInfoWidget);
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
		PositionPopupAtCursor(CardInfoWidget);
	}
}

void UBattlePlayerHUDWidget::HandleCardSlotUnhovered(int32 SlotNumber)
{
	if (IsValid(CardInfoWidget))
	{
		CardInfoWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBattlePlayerHUDWidget::PositionPopupAtCursor(UUserWidget* PopupWidget)
{
	if (!IsValid(PopupWidget))
	{
		return;
	}

	UCanvasPanelSlot* PopupCanvasSlot = Cast<UCanvasPanelSlot>(PopupWidget->Slot);
	APlayerController* OwningPC = GetOwningPlayer();
	if (!IsValid(PopupCanvasSlot) || !IsValid(OwningPC))
	{
		return;
	}

	FVector2D CursorPosition;
	if (!UWidgetLayoutLibrary::GetMousePositionScaledByDPI(OwningPC, CursorPosition.X, CursorPosition.Y))
	{
		return;
	}

	const FVector2D PopupOffset(16.0f, 16.0f);
	PopupCanvasSlot->SetPosition(CursorPosition + PopupOffset);
}

