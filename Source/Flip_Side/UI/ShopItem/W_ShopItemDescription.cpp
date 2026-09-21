// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemDescription.h"
#include "DataTypes/ItemDataTypes.h"

void UW_ShopItemDescription::NativeConstruct()
{
    Super::NativeConstruct();

    ResetItemWidget();
}

void UW_ShopItemDescription::SetItemInfo(const FItemData& ItemData)
{
    UpdateItemInfo(ItemData);
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UW_ShopItemDescription::ResetItemWidget()
{
    UpdateItemInfo(nullptr, FText::GetEmpty(), FText::GetEmpty(), FLinearColor::White);
    SetVisibility(ESlateVisibility::Collapsed);
}
