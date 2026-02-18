// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopItemBuyButton.h"
#include "Subsystem/ShopItemWSubsystem.h"
#include "Components/Button.h"
void UW_ShopItemBuyButton::NativeConstruct()
{
    Super::NativeConstruct();

    ShopSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    SelectButton->OnClicked.AddDynamic(this, &UW_ShopItemBuyButton::SelectItem);
    SelectButton->OnHovered.AddDynamic(this, &UW_ShopItemBuyButton::HoverItem);
    SelectButton->OnUnhovered.AddDynamic(this, &UW_ShopItemBuyButton::UnHoverItem);
    UnSelectButton->OnClicked.AddDynamic(this, &UW_ShopItemBuyButton::UnSelectItem);
}


void UW_ShopItemBuyButton::SelectItem()
{
    //ShopSubsystem->AddItem(SelectButtonNum);
}

void UW_ShopItemBuyButton::HoverItem()
{
    //ShopSubsystem->HoverShopItem(SelectButtonNum);
}

void UW_ShopItemBuyButton::UnHoverItem()
{
    //ShopSubsystem->UnHoverShopItem();
}

void UW_ShopItemBuyButton::UnSelectItem()
{
    //ShopSubsystem->ReduceItem(SelectButtonNum);
}
