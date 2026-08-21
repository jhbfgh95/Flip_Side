// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "CoinDataTypes.h"
#include "ItemDataTypes.h"
#include "CardTypes.h"
#include "BattlePlayerHUDWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDCoinSlotClicked, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDCoinSlotHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDCoinSlotUnhovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDReadyCoinClicked, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDReadyCoinHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDReadyCoinUnhovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDItemSlotClicked, int32);
DECLARE_MULTICAST_DELEGATE(FOnBattleHUDPhaseProgressClicked);

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UBattlePlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetCoinSlots(const TArray<FBattleCoinSlotViewData>& InCoinSlots);
	void SetReadyCoins(const TArray<FBattleReadyCoinViewData>& InReadyCoins);
	void SetBossHUDData(const FBossHUDData& InData);
	void SetItemSlots(const TArray<FBattleItemSlotViewData>& InItemSlots);
	void SetCardSlots(const TArray<FBattleCardSlotViewData>& InCardSlots);
	void SetPhaseDisplay(EPhaseState CurrentPhase, int32 TurnCount);
	void PlayBossPhaseCompletionAnimation();
	void ShowBattleCoinInfo(const FBattleCoinInfoViewData& InData, bool bUseReadyCoinAnchor);
	void HideBattleCoinInfo();
	void SetAdditionalBattleCoinBuffsVisible(bool bVisible);
	void SetCoinSlotDetailedStatsVisible(bool bVisible);

	FOnBattleHUDCoinSlotClicked OnCoinSlotClicked;
	FOnBattleHUDCoinSlotHovered OnCoinSlotHovered;
	FOnBattleHUDCoinSlotUnhovered OnCoinSlotUnhovered;
	FOnBattleHUDReadyCoinClicked OnReadyCoinClicked;
	FOnBattleHUDReadyCoinHovered OnReadyCoinHovered;
	FOnBattleHUDReadyCoinUnhovered OnReadyCoinUnhovered;
	FOnBattleHUDItemSlotClicked OnItemSlotClicked;
	FOnBattleHUDPhaseProgressClicked OnPhaseProgressClicked;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UW_BossHP> BossHPWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleBossPatternHUDWidget> BossPatternWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UW_BattlePhaseAndTurnDisplayUI> PhaseAndTurnDisplayWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> CoinSlotContainer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleReadyCoinWidget> BattleReadyCoinWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> PopupLayer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> CoinSlotPopupAnchor;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> ReadyCoinPopupAnchor;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> BattleCoinPopupAnchor;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> ItemPopupAnchor;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> CardPopupAnchor;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleItemSlotWidget> ItemSlot1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleItemSlotWidget> ItemSlot2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleItemSlotWidget> ItemSlot3;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleCardSlotWidget> CardSlot1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleCardSlotWidget> CardSlot2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleCardSlotWidget> CardSlot3;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Coin")
	TSubclassOf<class UBattleCoinSlotWidget> BattleCoinSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Coin")
	TSubclassOf<class UW_CoinSlotInfo> CoinSlotInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Coin")
	TSubclassOf<class UW_BattleCoinInfo> BattleCoinInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Item")
	TSubclassOf<class UW_ItemInfo> ItemInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Card")
	TSubclassOf<class UW_CardWidget> CardInfoWidgetClass;

private:
	void EnsureCoinSlotWidgets(int32 RequiredCount);
	void CacheFixedItemSlots();
	void CacheFixedCardSlots();
	void HandleCoinSlotClicked(int32 SlotNumber);
	void HandleCoinSlotHovered(int32 SlotNumber);
	void HandleCoinSlotUnhovered(int32 SlotNumber);
	void HandleReadyCoinClicked(int32 CoinInstanceID);
	void HandleReadyCoinHovered(int32 CoinInstanceID);
	void HandleReadyCoinUnhovered(int32 CoinInstanceID);
	void HandlePhaseProgressRequested();
	void HandleItemSlotClicked(int32 ItemID);
	void HandleItemSlotHovered(int32 ItemID);
	void HandleItemSlotUnhovered(int32 ItemID);
	void HandleCardSlotHovered(int32 SlotNumber);
	void HandleCardSlotUnhovered(int32 SlotNumber);
	void ApplyPopupAnchorLayout(class UUserWidget* PopupWidget, class UWidget* PopupAnchor);

	UPROPERTY()
	TArray<TObjectPtr<class UBattleCoinSlotWidget>> CoinSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<class UBattleItemSlotWidget>> ItemSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<class UBattleCardSlotWidget>> CardSlotWidgets;

	UPROPERTY()
	TObjectPtr<class UW_ItemInfo> ItemInfoWidget;

	UPROPERTY()
	TObjectPtr<class UW_CoinSlotInfo> CoinSlotInfoWidget;

	UPROPERTY()
	TObjectPtr<class UW_BattleCoinInfo> BattleCoinInfoWidget;

	UPROPERTY()
	TObjectPtr<class UW_CardWidget> CardInfoWidget;

	TMap<int32, FBattleCoinSlotViewData> CoinSlotViewDataByNumber;
	TMap<int32, FBattleItemSlotViewData> ItemSlotViewDataByID;
	TMap<int32, FBattleCardSlotViewData> CardSlotViewDataByNumber;
	bool bShowCoinSlotDetailedStats = false;
};
