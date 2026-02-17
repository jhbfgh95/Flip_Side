// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopItemInvenButton.h"
#include "Subsystem/ShopItemWSubsystem.h"
#include "Components/Button.h"

void UW_ShopItemInvenButton::NativeConstruct()
{
    Super::NativeConstruct();

    ShopSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    SelectButton->OnClicked.AddDynamic(this, &UW_ShopItemInvenButton::SelectItem);
    SelectButton->OnHovered.AddDynamic(this, &UW_ShopItemInvenButton::HoverItem);
    SelectButton->OnUnhovered.AddDynamic(this, &UW_ShopItemInvenButton::UnHoverItem);
    UnSelectButton->OnClicked.AddDynamic(this, &UW_ShopItemInvenButton::UnSelectItem);
}


void UW_ShopItemInvenButton::SelectItem()
{
    //ShopSubsystem->AddItemByInven(SelectButtonNum);
}

void UW_ShopItemInvenButton::HoverItem()
{
    //ShopSubsystem->HoverShopItem(SelectButtonNum);
}

void UW_ShopItemInvenButton::UnHoverItem()
{
    //ShopSubsystem->UnHoverShopItem();
}

void UW_ShopItemInvenButton::UnSelectItem()
{
    //ShopSubsystem->ReduceItemByInven(SelectButtonNum);
}
