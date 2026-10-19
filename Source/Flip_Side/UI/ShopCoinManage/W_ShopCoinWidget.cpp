// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinWidget.h"
#include "Components/Button.h"

void UW_ShopCoinWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsValid(RemoveCurrentWeaponButton))
	{
		RemoveCurrentWeaponButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleRemoveCurrentWeaponClicked);
	}
	if (IsValid(RemoveAllWeaponsButton))
	{
		RemoveAllWeaponsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleRemoveAllWeaponsClicked);
	}
}

void UW_ShopCoinWidget::HandleRemoveCurrentWeaponClicked()
{
	OnRemoveCurrentWeaponRequested.Broadcast();
}

void UW_ShopCoinWidget::HandleRemoveAllWeaponsClicked()
{
	OnRemoveAllWeaponsRequested.Broadcast();
}

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
	
UW_ShopSlotCoinDes* UW_ShopCoinWidget::GetWeaponDescription() const
{
	return WeaponDes;
}
