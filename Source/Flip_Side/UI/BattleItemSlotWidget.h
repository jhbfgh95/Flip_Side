// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDataTypes.h"
#include "BattleItemSlotWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleItemSlotWidgetClicked, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleItemSlotWidgetHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleItemSlotWidgetUnhovered, int32);

UCLASS()
class FLIP_SIDE_API UBattleItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetItemData(const FBattleItemSlotViewData& InData);
	void ClearItemData();

	FOnBattleItemSlotWidgetClicked OnBattleItemSlotClicked;
	FOnBattleItemSlotWidgetHovered OnBattleItemSlotHovered;
	FOnBattleItemSlotWidgetUnhovered OnBattleItemSlotUnhovered;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> ItemButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ItemCountText;

private:
	UFUNCTION()
	void HandleItemButtonClicked();

	UFUNCTION()
	void HandleItemButtonHovered();

	UFUNCTION()
	void HandleItemButtonUnhovered();

	int32 ItemID = INDEX_NONE;
	int32 AvailableCount = 0;
	bool bCanUse = false;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> ItemIconMaterialInstance;
};
