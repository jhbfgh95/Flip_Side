// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinDataTypes.h"
#include "BattleReadyCoinWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleReadyCoinWidgetClicked, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleReadyCoinWidgetHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleReadyCoinWidgetUnhovered, int32);

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UBattleReadyCoinWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void ShowBattleCoinInfo(const FBattleCoinInfoViewData& InData);
	void ClearBattleCoinInfo();
	void ToggleDescriptionDetails();
	bool IsInfoPageVisible() const { return bInfoPageVisible; }
	// HUD의 공통 팝업 영역 판정은 레디 영역 전체가 아닌 실제 슬롯만 사용합니다.
	const TArray<TObjectPtr<class UReadyCoinSlot>>& GetReadyCoinSlots() const { return ReadyCoinSlots; }
	// HUD → Controller: 수동 페이지 전환은 선택 대상/슬롯 윤곽선을 해제합니다.
	FSimpleMulticastDelegate OnInfoSelectionReset;
	FSimpleMulticastDelegate OnSlotHighlightClearRequested;

	void SetReadyCoins(const TArray<FBattleReadyCoinViewData>& InReadyCoins);

	FOnBattleReadyCoinWidgetClicked OnReadyCoinClicked;
	FOnBattleReadyCoinWidgetHovered OnReadyCoinHovered;
	FOnBattleReadyCoinWidgetUnhovered OnReadyCoinUnhovered;

protected:
	// BP Switcher의 0번은 슬롯 영역, 1번은 BattleCoinInfoWidget을 포함한 정보 영역입니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidgetSwitcher> ReadyInfoSwitcher;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleCoinInfoWidget> BattleCoinInfoWidget;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ReadyAreaTitleText;
	// 전체 수량/구분자 등은 이 컨테이너 아래에 묶으면 정보 모드에서 함께 숨깁니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> CoinCountContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> TotalCoinCountText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ReadyCoinCountText;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Ready Coin|Count", meta = (DisplayName = "Count Color (0-2)"))
	FLinearColor ReadyCoinCountLowColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Ready Coin|Count", meta = (DisplayName = "Count Color (3-6)"))
	FLinearColor ReadyCoinCountMediumColor = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Ready Coin|Count", meta = (DisplayName = "Count Color (7-10)"))
	FLinearColor ReadyCoinCountHighColor = FLinearColor::Red;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot7;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot8;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot9;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UReadyCoinSlot> ReadyCoinSlot10;

private:
	void HandleBackToReadyClicked();
	void SetInfoPageVisible(bool bVisible);
	bool bInfoPageVisible = false;
	void UpdateReadyCoinCountText(int32 FilledSlotCount);
	void CacheReadyCoinSlots();
	void HandleReadyCoinSlotClicked(int32 CoinInstanceID);
	void HandleReadyCoinSlotHovered(int32 CoinInstanceID);
	void HandleReadyCoinSlotUnhovered(int32 CoinInstanceID);

	UPROPERTY()
	TArray<TObjectPtr<class UReadyCoinSlot>> ReadyCoinSlots;
};
