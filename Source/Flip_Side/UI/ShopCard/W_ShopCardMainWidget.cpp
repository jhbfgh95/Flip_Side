// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardMainWidget.h"

UW_ShopCardSlotContainer* UW_ShopCardMainWidget::GetShopCardSlotContainer() const
{
	return ShopCardSlotContainer;
}

UW_ShopPlayerCardSlotContainer* UW_ShopCardMainWidget::GetShopPlayerCardSlotContainer() const
{
	return ShopPlayerCardSlotContainer;
}
