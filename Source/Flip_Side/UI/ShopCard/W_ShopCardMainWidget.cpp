// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardMainWidget.h"
#include "UI/ShopCard/W_ShopCardWidget.h"

UW_ShopCardSlotContainer* UW_ShopCardMainWidget::GetShopCardSlotContainer() const
{
	return ShopCardSlotContainer;
}

UW_ShopPlayerCardSlotContainer* UW_ShopCardMainWidget::GetShopPlayerCardSlotContainer() const
{
	return ShopPlayerCardSlotContainer;
}

UW_ShopCardWidget* UW_ShopCardMainWidget::GetShopCardWidget() const
{
	return ShopCardWidget;
}
