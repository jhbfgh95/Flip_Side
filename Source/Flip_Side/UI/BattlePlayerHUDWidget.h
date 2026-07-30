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
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleHUDItemSlotClicked, int32);

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

	FOnBattleHUDCoinSlotClicked OnCoinSlotClicked;
	FOnBattleHUDCoinSlotHovered OnCoinSlotHovered;
	FOnBattleHUDCoinSlotUnhovered OnCoinSlotUnhovered;
	FOnBattleHUDReadyCoinClicked OnReadyCoinClicked;
	FOnBattleHUDItemSlotClicked OnItemSlotClicked;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UW_BossHP> BossHPWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> CoinSlotContainer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleReadyCoinWidget> BattleReadyCoinWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> ItemSlotContainer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> PopupLayer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> CardSlotContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Coin")
	TSubclassOf<class UBattleCoinSlotWidget> BattleCoinSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Item")
	TSubclassOf<class UBattleItemSlotWidget> BattleItemSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Item")
	TSubclassOf<class UW_ItemInfo> ItemInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Card")
	TSubclassOf<class UBattleCardSlotWidget> BattleCardSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Battle HUD|Card")
	TSubclassOf<class UW_CardWidget> CardInfoWidgetClass;

private:
	void EnsureCoinSlotWidgets(int32 RequiredCount);
	void EnsureItemSlotWidgets(int32 RequiredCount);
	void EnsureCardSlotWidgets(int32 RequiredCount);
	void HandleCoinSlotClicked(int32 SlotNumber);
	void HandleCoinSlotHovered(int32 SlotNumber);
	void HandleCoinSlotUnhovered(int32 SlotNumber);
	void HandleReadyCoinClicked(int32 CoinInstanceID);
	void HandleItemSlotClicked(int32 ItemID);
	void HandleItemSlotHovered(int32 ItemID);
	void HandleItemSlotUnhovered(int32 ItemID);
	void HandleCardSlotHovered(int32 SlotNumber);
	void HandleCardSlotUnhovered(int32 SlotNumber);
	void PositionPopupAtCursor(class UUserWidget* PopupWidget);

	UPROPERTY()
	TArray<TObjectPtr<class UBattleCoinSlotWidget>> CoinSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<class UBattleItemSlotWidget>> ItemSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<class UBattleCardSlotWidget>> CardSlotWidgets;

	UPROPERTY()
	TObjectPtr<class UW_ItemInfo> ItemInfoWidget;

	UPROPERTY()
	TObjectPtr<class UW_CardWidget> CardInfoWidget;

	TMap<int32, FBattleItemSlotViewData> ItemSlotViewDataByID;
	TMap<int32, FBattleCardSlotViewData> CardSlotViewDataByNumber;
};
