// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopItem/W_ShopItemPurchasePopup.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UW_ShopItemPurchasePopup::NativeConstruct()
{
	Super::NativeConstruct();

	CountPlusButton->OnClicked.AddDynamic(this, &ThisClass::ClickPlus);
	CountMinusButton->OnClicked.AddDynamic(this, &ThisClass::ClickMinus);
	PurchaseButton->OnClicked.AddDynamic(this, &ThisClass::ClickConfirm);
	CancelButton->OnClicked.AddDynamic(this, &ThisClass::ClickCancel);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopItemPurchasePopup::Open(const FItemData& InItemData)
{
	CurrentItemData = InItemData;
	CurrentCount = 1;

	ItemImage->SetBrushFromTexture(CurrentItemData.ItemIcon);
	ItemNameText->SetText(FText::FromString(CurrentItemData.ItemName));
	ItemCountText->SetText(FText::AsNumber(CurrentCount));
	TotalPriceText->SetText(FText::AsNumber(CurrentItemData.Price * CurrentCount));
	SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopItemPurchasePopup::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopItemPurchasePopup::ClickPlus()
{
	++CurrentCount;
	ItemCountText->SetText(FText::AsNumber(CurrentCount));
	TotalPriceText->SetText(FText::AsNumber(CurrentItemData.Price * CurrentCount));
}

void UW_ShopItemPurchasePopup::ClickMinus()
{
	CurrentCount = FMath::Max(1, CurrentCount - 1);
	ItemCountText->SetText(FText::AsNumber(CurrentCount));
	TotalPriceText->SetText(FText::AsNumber(CurrentItemData.Price * CurrentCount));
}

void UW_ShopItemPurchasePopup::ClickConfirm()
{
	OnPurchaseItemRequested.Broadcast(CurrentItemData.ItemID, CurrentCount);
}

void UW_ShopItemPurchasePopup::ClickCancel()
{
	OnCancelled.Broadcast();
}
