// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopItem/W_ShopItemSellPopup.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UW_ShopItemSellPopup::NativeConstruct()
{
    Super::NativeConstruct();

    CountPlusButton->OnClicked.AddDynamic(this, &ThisClass::ClickPlus);
    CountMinusButton->OnClicked.AddDynamic(this, &ThisClass::ClickMinus);
    SellButton->OnClicked.AddDynamic(this, &ThisClass::ClickSell);
    CancelButton->OnClicked.AddDynamic(this, &ThisClass::ClickCancel);
    SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopItemSellPopup::Open(int32 InInventoryIndex, const FItemData& InItemData, int32 InMaxSellCount)
{
    if (InInventoryIndex == INDEX_NONE || InItemData.ItemID == -1 || InMaxSellCount <= 0)
        return;

    CurrentInventoryIndex = InInventoryIndex;
    CurrentItemData = InItemData;
    MaxSellCount = InMaxSellCount;
    CurrentCount = 1;

    ItemImage->SetBrushFromTexture(CurrentItemData.ItemIcon);
    ItemNameText->SetText(FText::FromString(CurrentItemData.ItemName));
    OwnedCountText->SetText(FText::AsNumber(MaxSellCount));
    RefreshCountAndPrice();
    SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopItemSellPopup::Close()
{
    SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopItemSellPopup::RefreshCountAndPrice()
{
    ItemCountText->SetText(FText::AsNumber(CurrentCount));
    TotalPriceText->SetText(FText::AsNumber(CurrentItemData.Price * CurrentCount));
}

void UW_ShopItemSellPopup::ClickPlus()
{
    CurrentCount = FMath::Min(CurrentCount + 1, MaxSellCount);
    RefreshCountAndPrice();
}

void UW_ShopItemSellPopup::ClickMinus()
{
    CurrentCount = FMath::Max(CurrentCount - 1, 1);
    RefreshCountAndPrice();
}

void UW_ShopItemSellPopup::ClickSell()
{
    if (CurrentInventoryIndex != INDEX_NONE && CurrentCount > 0 && CurrentCount <= MaxSellCount)
    {
        OnSellItemRequested.Broadcast(CurrentInventoryIndex, CurrentItemData.ItemID, CurrentCount);
    }
}

void UW_ShopItemSellPopup::ClickCancel()
{
    OnCancelled.Broadcast();
}
