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

	void SetReadyCoins(const TArray<FBattleReadyCoinViewData>& InReadyCoins);

	FOnBattleReadyCoinWidgetClicked OnReadyCoinClicked;
	FOnBattleReadyCoinWidgetHovered OnReadyCoinHovered;
	FOnBattleReadyCoinWidgetUnhovered OnReadyCoinUnhovered;

protected:
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
	void CacheReadyCoinSlots();
	void HandleReadyCoinSlotClicked(int32 CoinInstanceID);
	void HandleReadyCoinSlotHovered(int32 CoinInstanceID);
	void HandleReadyCoinSlotUnhovered(int32 CoinInstanceID);

	UPROPERTY()
	TArray<TObjectPtr<class UReadyCoinSlot>> ReadyCoinSlots;
};
