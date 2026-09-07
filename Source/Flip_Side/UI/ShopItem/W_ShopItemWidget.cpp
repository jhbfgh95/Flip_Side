// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemWidget.h"

#include "UI/ShopItem/W_ShopItemDescription.h"
#include "UI/ShopItem/W_ShopItemPurchasePopup.h"
#include "UI/ShopItem/W_ShopItemSellPopup.h"

UW_ShopPlayerItemSlotContainer* UW_ShopItemWidget::GetShopPlayerItemSlotContainer() const
{
	return ShopPlayerItemSlotContainer;
}

UW_ShopItemSlotContainer* UW_ShopItemWidget::GetShopItemSlotContainer() const
{
	return ShopItemSlotContainer;
}

UW_ShopItemDescription* UW_ShopItemWidget::GetShopItemDescription() const
{
	return ShopItemDescription;
}

UW_ShopItemPurchasePopup* UW_ShopItemWidget::GetShopItemPurchasePopup() const
{
	return ShopItemPurchasePopup;
}

UW_ShopItemSellPopup* UW_ShopItemWidget::GetShopItemSellPopup() const
{
	return ShopItemSellPopup;
}

