// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemWidget.h"

#include "UI/ShopItem/W_ShopItemDescription.h"

UW_ShopPlayerItemSlotContainer* UW_ShopItemWidget::GetShopPlayerItemSlotContainer() const
{
	return ShopPlayerItemSlotContainer;
}

UW_ShopItemSlotContainer* UW_ShopItemWidget::GetShopItemSlotContainer() const
{
	return ShopItemSlotContainer;
}

UW_ShopSelectedItem* UW_ShopItemWidget::GetShopSelectedItem() const
{
	return ShopSelectedItem;
}

UW_ShopItemDescription* UW_ShopItemWidget::GetShopItemDescription() const
{
	return ShopItemDescription;
}

