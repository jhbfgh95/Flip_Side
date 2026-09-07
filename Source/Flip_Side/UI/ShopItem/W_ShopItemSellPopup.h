// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemSellPopup.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSellItemRequested, int32, InventoryIndex, int32, ItemID, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSellPopupCancelled);

UCLASS()
class FLIP_SIDE_API UW_ShopItemSellPopup : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemNameText;

    // 현재 보유 수량을 표시합니다.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> OwnedCountText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemCountText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TotalPriceText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CountPlusButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CountMinusButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SellButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CancelButton;

    FItemData CurrentItemData;
    int32 CurrentInventoryIndex = INDEX_NONE;
    int32 MaxSellCount = 0;
    int32 CurrentCount = 1;

public:
    FOnSellItemRequested OnSellItemRequested;
    FOnSellPopupCancelled OnCancelled;

    void Open(int32 InInventoryIndex, const FItemData& InItemData, int32 InMaxSellCount);
    void Close();

private:
    void RefreshCountAndPrice();

    UFUNCTION() void ClickPlus();
    UFUNCTION() void ClickMinus();
    UFUNCTION() void ClickSell();
    UFUNCTION() void ClickCancel();
};
