// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDataTypes.h"
#include "Styling/SlateTypes.h"
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
	virtual void NativeDestruct() override;
	void SetInfoSelected(bool bSelected);
	int32 GetItemID() const { return ItemID; }

	void SetItemData(const FBattleItemSlotViewData& InData);
	void ClearItemData();

	FOnBattleItemSlotWidgetClicked OnBattleItemSlotClicked;
	FOnBattleItemSlotWidgetHovered OnBattleItemSlotHovered;
	FOnBattleItemSlotWidgetUnhovered OnBattleItemSlotUnhovered;

protected:
	// 물리적인 호버가 아닌, 현재 팝업에 표시 중인 아이템의 선택 상태입니다.
	UPROPERTY(BlueprintReadOnly, Category = "Battle Item Slot|Selection")
	bool bIsInfoSelected = false;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Item Slot|Selection")
	bool bUseHoveredStyleForInfoSelection = true;
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Item Slot|Selection")
	void OnInfoSelectionChanged(bool bSelected);
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> ItemButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ItemCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ItemNameText;

private:
	void RefreshInfoSelectionStyle();
	UPROPERTY(Transient)
	FButtonStyle UnselectedButtonStyle;
	bool bHasUnselectedButtonStyle = false;
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
