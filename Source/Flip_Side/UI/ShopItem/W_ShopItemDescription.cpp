// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemDescription.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"


void UW_ShopItemDescription::NativeConstruct()
{
    Super::NativeConstruct();
}
void UW_ShopItemDescription::SetItemInfo(FString ItemName, FString Description)
{
    
    ItemDesText->SetText(FText::FromString(Description));

    ItemNameText->SetText(FText::FromString(ItemName));

}


void UW_ShopItemDescription::ResetItemWidget()
{
    SetItemInfo(FString(""),FString(""));
}