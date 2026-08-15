// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemWidget.h"

UW_ShopPlayerItemSlotContainer* UW_ShopItemWidget::GetShopPlayerItemSlotContainer() const
{
	return ShopPlayerItemSlotContainer;
}

UW_ShopItemSlotContainer* UW_ShopItemWidget::GetShopItemSlotContainer() const
{
	return ShopItemSlotContainer;
}

