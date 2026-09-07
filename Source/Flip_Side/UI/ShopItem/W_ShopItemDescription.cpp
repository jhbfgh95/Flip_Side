// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemDescription.h"
#include "Components/TextBlock.h"


void UW_ShopItemDescription::NativeConstruct()
{
    Super::NativeConstruct();

    ResetItemWidget();
}

void UW_ShopItemDescription::SetItemInfo(const FString& ItemName, const FString& Description)
{
    if (ItemDesText)
    {
        ItemDesText->SetText(FText::FromString(Description));
    }

    if (ItemNameText)
    {
        ItemNameText->SetText(FText::FromString(ItemName));
    }

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}


void UW_ShopItemDescription::ResetItemWidget()
{
    if (ItemDesText)
    {
        ItemDesText->SetText(FText::GetEmpty());
    }

    if (ItemNameText)
    {
        ItemNameText->SetText(FText::GetEmpty());
    }

    SetVisibility(ESlateVisibility::Collapsed);
}
