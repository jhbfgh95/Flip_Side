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
#include "UI/BossPatternPopupWidget.h"
#include "UI/W_BattlePhaseAndTurnDisplayUI.h"
#include "UI/W_Battle_Lever.h"
#include "UI/CoinSlotPopupInputProcessor.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"

void UBattlePlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(BossPatternWidget))
	{
		BossPatternWidget->OnPatternHovered.RemoveAll(this);
		BossPatternWidget->OnPatternUnhovered.RemoveAll(this);
		BossPatternWidget->OnPatternHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleBossPatternHovered);
		BossPatternWidget->OnPatternUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleBossPatternUnhovered);
	}

	if (IsValid(BattleReadyCoinWidget))
	{
		BattleReadyCoinWidget->OnReadyCoinClicked.RemoveAll(this);
		BattleReadyCoinWidget->OnReadyCoinHovered.RemoveAll(this);
		BattleReadyCoinWidget->OnReadyCoinUnhovered.RemoveAll(this);
		BattleReadyCoinWidget->OnReadyCoinClicked.AddUObject(this, &UBattlePlayerHUDWidget::HandleReadyCoinClicked);
		BattleReadyCoinWidget->OnReadyCoinHovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleReadyCoinHovered);
		BattleReadyCoinWidget->OnReadyCoinUnhovered.AddUObject(this, &UBattlePlayerHUDWidget::HandleReadyCoinUnhovered);
		BattleReadyCoinWidget->OnInfoSelectionReset.RemoveAll(this);
		BattleReadyCoinWidget->OnSlotHighlightClearRequested.RemoveAll(this);
		BattleReadyCoinWidget->OnInfoSelectionReset.AddUObject(this, &UBattlePlayerHUDWidget::HandleBattleInfoSelectionReset);
		BattleReadyCoinWidget->OnSlotHighlightClearRequested.AddUObject(this, &UBattlePlayerHUDWidget::HandleReadySlotHighlightClearRequested);
	}

	if (IsValid(LeverWidget))
	{
		LeverWidget->OnPhaseProgressRequested.RemoveAll(this);
		LeverWidget->OnPhaseProgressRequested.AddUObject(this, &UBattlePlayerHUDWidget::HandlePhaseProgressRequested);
	}

	CacheFixedItemSlots();
	CacheFixedCardSlots();
	if (FSlateApplication::IsInitialized() && !CoinPopupInputProcessor.IsValid())
	{
		CoinPopupInputProcessor = MakeShared<FCoinSlotPopupInputProcessor>(this);
		FSlateApplication::Get().RegisterInputPreProcessor(CoinPopupInputProcessor, 0);
	}
}

void UBattlePlayerHUDWidget::NativeDestruct()
{
	HandleBossPatternUnhovered();
	if (IsValid(BossPatternWidget))
	{
		BossPatternWidget->OnPatternHovered.RemoveAll(this);
		BossPatternWidget->OnPatternUnhovered.RemoveAll(this);
	}
	HandleBattleInfoSelectionReset();
	HandleReadySlotHighlightClearRequested();
	if (IsValid(BattleReadyCoinWidget))
	{
		BattleReadyCoinWidget->OnInfoSelectionReset.RemoveAll(this);
		BattleReadyCoinWidget->OnSlotHighlightClearRequested.RemoveAll(this);
		BattleReadyCoinWidget->OnReadyCoinClicked.RemoveAll(this);
		BattleReadyCoinWidget->OnReadyCoinHovered.RemoveAll(this);
		BattleReadyCoinWidget->OnReadyCoinUnhovered.RemoveAll(this);
	}
	DismissCoinSlotInfo();
	DismissItemInfo();
	if (IsValid(ItemInfoWidget)) ItemInfoWidget->OnCloseRequested.RemoveAll(this);
	if (IsValid(CoinSlotInfoWidget)) CoinSlotInfoWidget->OnCloseRequested.RemoveAll(this);
	if (FSlateApplication::IsInitialized() && CoinPopupInputProcessor.IsValid())
		FSlateApplication::Get().UnregisterInputPreProcessor(CoinPopupInputProcessor);
	CoinPopupInputProcessor.Reset();
	Super::NativeDestruct();
}

bool UBattlePlayerHUDWidget::IsCoinSlotInfoOpen() const
{
	return IsVisible() && DisplayedCoinSlotNumber != INDEX_NONE &&
		IsValid(CoinSlotInfoWidget) && CoinSlotInfoWidget->IsVisible();
}

bool UBattlePlayerHUDWidget::IsItemInfoOpen() const
{
	return IsVisible() && DisplayedItemID != INDEX_NONE && IsValid(ItemInfoWidget) && ItemInfoWidget->IsVisible();
}

void UBattlePlayerHUDWidget::DismissItemInfo()
{
	DisplayedItemID = INDEX_NONE;
	if (IsValid(ItemInfoWidget)) ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	RefreshItemInfoSelection();
}

void UBattlePlayerHUDWidget::RefreshItemInfoSelection()
{
	const bool bOpen = IsItemInfoOpen();
	for (UBattleItemSlotWidget* ItemSlot : ItemSlotWidgets)
		if (IsValid(ItemSlot)) ItemSlot->SetInfoSelected(bOpen && ItemSlot->GetItemID() == DisplayedItemID);
}

void UBattlePlayerHUDWidget::DismissCoinSlotInfo()
{
	const int32 PreviousSlot = DisplayedCoinSlotNumber;
	DisplayedCoinSlotNumber = INDEX_NONE;
	RefreshCoinSlotInfoSelection();
	if (IsValid(CoinSlotInfoWidget))
	{
		CoinSlotInfoWidget->ResetDetailedDescriptions();
		CoinSlotInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PreviousSlot != INDEX_NONE) OnCoinSlotInfoDismissed.Broadcast(PreviousSlot);
}

void UBattlePlayerHUDWidget::RegisterCoinInfoDismissRegion(UWidget* Widget)
{
	if (IsValid(Widget)) CoinInfoDismissRegions.AddUnique(TWeakObjectPtr<UWidget>(Widget));
}

void UBattlePlayerHUDWidget::UnregisterCoinInfoDismissRegion(UWidget* Widget)
{
	CoinInfoDismissRegions.RemoveAll([Widget](const TWeakObjectPtr<UWidget>& Entry)
	{
		return !Entry.IsValid() || Entry.Get() == Widget;
	});
}

void UBattlePlayerHUDWidget::ToggleCoinDescriptionDetails()
{
	if (IsCoinSlotInfoOpen()) CoinSlotInfoWidget->ToggleDetailedDescriptions();
	else if (IsValid(BattleReadyCoinWidget)) BattleReadyCoinWidget->ToggleDescriptionDetails();
}

ECoinPopupPointerRegion UBattlePlayerHUDWidget::GetCoinPopupPointerRegion(const FVector2D& ScreenPosition) const
{
	if (!FSlateApplication::IsInitialized() || !GetCachedWidget().IsValid() ||
		!GetCachedGeometry().IsUnderLocation(ScreenPosition)) return ECoinPopupPointerRegion::OutsideGame;
	FSlateApplication& App = FSlateApplication::Get();
	const FWidgetPath Path = App.LocateWindowUnderMouse(ScreenPosition, App.GetInteractiveTopLevelWindows());
	if (!Path.IsValid() || Path.GetWindow() != App.FindWidgetWindow(GetCachedWidget().ToSharedRef()))
		return ECoinPopupPointerRegion::OutsideGame;
	auto InPath = [&Path](const UWidget* Widget)
	{
		return IsValid(Widget) && Widget->GetCachedWidget().IsValid() && Path.ContainsWidget(Widget->GetCachedWidget().Get());
	};
	if (InPath(CoinSlotInfoWidget)) return ECoinPopupPointerRegion::CoinUI;
	for (const UBattleCoinSlotWidget* CoinSlot : CoinSlotWidgets)
		if (InPath(CoinSlot)) return ECoinPopupPointerRegion::CoinUI;
	if (InPath(ItemInfoWidget)) return ECoinPopupPointerRegion::ItemUI;
	for (const UBattleItemSlotWidget* Item : ItemSlotWidgets)
		if (InPath(Item)) return ECoinPopupPointerRegion::ItemUI;

	// 알려진 다른 UI와 명시적으로 등록한 영역만 닫기 대상으로 취급합니다.
	// ReadyCoin 전체 레이아웃은 제외합니다. 개별 슬롯 버튼/호버 이벤트에서 닫습니다.
	if (InPath(CardInfoWidget) || (IsValid(BattleReadyCoinWidget) && BattleReadyCoinWidget->IsInfoPageVisible() && InPath(BattleReadyCoinWidget)))
		return ECoinPopupPointerRegion::OtherUI;
	for (const UBattleCardSlotWidget* Card : CardSlotWidgets)
		if (InPath(Card)) return ECoinPopupPointerRegion::OtherUI;
	for (const TWeakObjectPtr<UWidget>& Region : CoinInfoDismissRegions)
		if (InPath(Region.Get())) return ECoinPopupPointerRegion::OtherUI;
	// SObjectWidget 자체는 레이아웃 래퍼일 수 있으므로 더 이상 닫기 근거로 삼지 않습니다.
	for (int32 Index = 0; Index < Path.Widgets.Num(); ++Index)
	{
		const FArrangedWidget& Entry = Path.Widgets[Index];
		const FName Type = Entry.Widget->GetType();
		if (Type == TEXT("SButton") || Type == TEXT("SCheckBox") || Type == TEXT("SSlider") ||
			Type == TEXT("SEditableText") || Type == TEXT("SEditableTextBox") || Type == TEXT("SScrollBox"))
			return ECoinPopupPointerRegion::OtherUI;
	}
	// 자식 장식의 hit-test 설정 때문에 슬롯/팝업 경로가 빠져도 내부 이동은 유지합니다.
	auto ContainsPoint = [&ScreenPosition](const UWidget* Widget)
	{
		return IsValid(Widget) && Widget->IsVisible() && Widget->GetCachedGeometry().IsUnderLocation(ScreenPosition);
	};
	if (IsCoinSlotInfoOpen() && ContainsPoint(CoinSlotInfoWidget)) return ECoinPopupPointerRegion::CoinUI;
	for (const UBattleCoinSlotWidget* CoinSlot : CoinSlotWidgets)
		if (ContainsPoint(CoinSlot)) return ECoinPopupPointerRegion::CoinUI;
	if (IsItemInfoOpen() && ContainsPoint(ItemInfoWidget)) return ECoinPopupPointerRegion::ItemUI;
	for (const UBattleItemSlotWidget* Item : ItemSlotWidgets)
		if (ContainsPoint(Item)) return ECoinPopupPointerRegion::ItemUI;
	return ECoinPopupPointerRegion::World;
}

void UBattlePlayerHUDWidget::RefreshCoinSlotInfoSelection()
{
	const bool bInfoOpen = IsCoinSlotInfoOpen();
	for (UBattleCoinSlotWidget* CoinSlot : CoinSlotWidgets)
	{
		if (IsValid(CoinSlot))
			CoinSlot->SetInfoSelected(bInfoOpen && CoinSlot->GetSlotNumber() == DisplayedCoinSlotNumber);
	}
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
	if (DisplayedCoinSlotNumber != INDEX_NONE)
	{
		if (const FBattleCoinSlotViewData* Data = CoinSlotViewDataByNumber.Find(DisplayedCoinSlotNumber);
			Data && IsValid(CoinSlotInfoWidget)) CoinSlotInfoWidget->SetCoinSlotInfo(*Data);
		else DismissCoinSlotInfo();
	}
	RefreshCoinSlotInfoSelection();
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
	CachedBossPatternHUDData = InData;
	if (!InData.bHasPatternInfo) HandleBossPatternUnhovered();
	else if (bBossPatternHovered && IsValid(BossPatternPopupWidget)) BossPatternPopupWidget->SetPatternInfo(InData);
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
	// 수량 갱신/슬롯 재배치에도 현재 표시 중인 아이템의 선택 외형을 유지합니다.
	RefreshItemInfoSelection();
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
	if (IsValid(LeverWidget))
	{
		LeverWidget->SetPhase(CurrentPhase);
	}

	if (IsValid(PhaseAndTurnDisplayWidget))
	{
		PhaseAndTurnDisplayWidget->SetPhaseDisplay(CurrentPhase, TurnCount);
	}
}

void UBattlePlayerHUDWidget::PlayBossPhaseCompletionAnimation()
{
	if (IsValid(LeverWidget))
	{
		LeverWidget->PlayBossPhaseCompletionAnimation();
	}
}

void UBattlePlayerHUDWidget::ShowBattleCoinInfo(
	const FBattleCoinInfoViewData& InData)
{
	// 배틀 인포는 팝업과 수명이 독립적인 레디 영역 하위 페이지입니다.
	if (IsValid(BattleReadyCoinWidget)) BattleReadyCoinWidget->ShowBattleCoinInfo(InData);
}

void UBattlePlayerHUDWidget::HideBattleCoinInfo()
{
	if (IsValid(BattleReadyCoinWidget)) BattleReadyCoinWidget->ClearBattleCoinInfo();
}

void UBattlePlayerHUDWidget::HandleBattleInfoSelectionReset()
{
	OnBattleInfoSelectionReset.Broadcast();
}

void UBattlePlayerHUDWidget::HandleReadySlotHighlightClearRequested()
{
	OnReadySlotHighlightClearRequested.Broadcast();
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
	DismissItemInfo();
	OnCoinSlotClicked.Broadcast(SlotNumber);
}

void UBattlePlayerHUDWidget::HandleCoinSlotHovered(int32 SlotNumber)
{
	DismissItemInfo();
	// 같은 슬롯 내부의 재호버는 데이터/스크롤/사거리 프리뷰를 다시 초기화하지 않습니다.
	if (IsCoinSlotInfoOpen() && DisplayedCoinSlotNumber == SlotNumber) return;
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
			const bool bWasOpen = IsCoinSlotInfoOpen();
			CoinSlotInfoWidget->OnCloseRequested.RemoveAll(this);
			CoinSlotInfoWidget->OnCloseRequested.AddUObject(this, &UBattlePlayerHUDWidget::DismissCoinSlotInfo);
			DisplayedCoinSlotNumber = SlotNumber;
			DismissItemInfo();
			if (IsValid(CardInfoWidget)) CardInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
			CoinSlotInfoWidget->SetCoinSlotInfo(*CoinSlotData);
			if (!bWasOpen) CoinSlotInfoWidget->ResetDescriptionSelection();
			CoinSlotInfoWidget->SetVisibility(ESlateVisibility::Visible);
			RefreshCoinSlotInfoSelection();
			ApplyPopupAnchorLayout(CoinSlotInfoWidget, CoinSlotPopupAnchor);
			OnCoinSlotHovered.Broadcast(SlotNumber);
		}
	}

}

void UBattlePlayerHUDWidget::HandleCoinSlotUnhovered(int32 SlotNumber)
{
	// 물리적인 Unhover 알림은 유지하되 표시 대상과 프리뷰 수명에는 영향을 주지 않습니다.
	OnCoinSlotUnhovered.Broadcast(SlotNumber);
}
void UBattlePlayerHUDWidget::HandleReadyCoinClicked(int32 CoinInstanceID)
{
	DismissCoinSlotInfo();
	DismissItemInfo();
	OnReadyCoinClicked.Broadcast(CoinInstanceID);
}

void UBattlePlayerHUDWidget::HandleReadyCoinHovered(int32 CoinInstanceID)
{
	DismissCoinSlotInfo();
	DismissItemInfo();
	if (CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinHovered.Broadcast(CoinInstanceID);
	}
}

void UBattlePlayerHUDWidget::HandleReadyCoinUnhovered(int32 CoinInstanceID)
{
	if (CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinUnhovered.Broadcast(CoinInstanceID);
	}
}

void UBattlePlayerHUDWidget::HandlePhaseProgressRequested()
{
	DismissCoinSlotInfo();
	DismissItemInfo();
	OnPhaseProgressClicked.Broadcast();
}

void UBattlePlayerHUDWidget::HandleItemSlotClicked(int32 ItemID)
{
	DismissCoinSlotInfo();
	OnItemSlotClicked.Broadcast(ItemID);
}

void UBattlePlayerHUDWidget::HandleItemSlotHovered(int32 ItemID)
{
	DismissCoinSlotInfo();
	// 같은 슬롯 재호버 시 키워드 사전/스크롤 등 사용자가 보던 상태를 유지합니다.
	if (IsItemInfoOpen() && DisplayedItemID == ItemID) return;
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
		ItemInfoWidget->OnCloseRequested.RemoveAll(this);
		ItemInfoWidget->OnCloseRequested.AddUObject(this, &UBattlePlayerHUDWidget::DismissItemInfo);
		DisplayedItemID = ItemID;
		if (IsValid(CardInfoWidget)) CardInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
		ItemInfoWidget->UpdateItemInfo(ItemSlotData->ItemData);
		ItemInfoWidget->SetVisibility(ESlateVisibility::Visible);
		RefreshItemInfoSelection();
		ApplyPopupAnchorLayout(ItemInfoWidget, ItemPopupAnchor);
	}
}

void UBattlePlayerHUDWidget::HandleItemSlotUnhovered(int32 ItemID)
{
	// 표시 데이터와 수명은 명시적인 닫기/다른 UI 상호작용으로만 변경합니다.
}

void UBattlePlayerHUDWidget::HandleCardSlotHovered(int32 SlotNumber)
{
	DismissCoinSlotInfo();
	DismissItemInfo();
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

void UBattlePlayerHUDWidget::HandleBossPatternHovered()
{
	if (!CachedBossPatternHUDData.bHasPatternInfo || !IsValid(PopupLayer) || !IsValid(BossPatternPopupAnchor) ||
		!BossPatternPopupWidgetClass || BossPatternPopupWidgetClass->HasAnyClassFlags(CLASS_Abstract)) return;
	DismissCoinSlotInfo();
	DismissItemInfo();
	if (IsValid(CardInfoWidget)) CardInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (!IsValid(BossPatternPopupWidget))
	{
		BossPatternPopupWidget = CreateWidget<UBossPatternPopupWidget>(this, BossPatternPopupWidgetClass);
		if (IsValid(BossPatternPopupWidget)) PopupLayer->AddChild(BossPatternPopupWidget);
	}
	if (!IsValid(BossPatternPopupWidget)) return;
	bBossPatternHovered = true;
	BossPatternPopupWidget->SetPatternInfo(CachedBossPatternHUDData);
	ApplyPopupAnchorLayout(BossPatternPopupWidget, BossPatternPopupAnchor);
}

void UBattlePlayerHUDWidget::HandleBossPatternUnhovered()
{
	bBossPatternHovered = false;
	if (IsValid(BossPatternPopupWidget)) BossPatternPopupWidget->SetVisibility(ESlateVisibility::Collapsed);
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

