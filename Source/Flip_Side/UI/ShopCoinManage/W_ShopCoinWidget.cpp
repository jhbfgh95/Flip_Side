// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinWidget.h"

#include "UI/W_WeaponDescription.h"

UW_ShopWeaponSlotContainer* UW_ShopCoinWidget::GetShopWeaponSlotContainer() const
{
	return ShopWeaponSlotContainer;
}

UW_ShopCoinSlotContainer* UW_ShopCoinWidget::GetShopCoinSlotContainer() const
{
	return ShopCoinSlotContainer;
}

UW_BuyCoinSlotContainer* UW_ShopCoinWidget::GetBuyCoinSlotContainer() const
{
	return BuyCoinSlotContainer;
}
	
UW_ShopSelectCoin* UW_ShopCoinWidget::GetShopSelectCoin()
{
	return ShopSelectCoin;
}

UW_WeaponDescription* UW_ShopCoinWidget::GetWeaponDescription() const
{
	return WeaponDes;
}
