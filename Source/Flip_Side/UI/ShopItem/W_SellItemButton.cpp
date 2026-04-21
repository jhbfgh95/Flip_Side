// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_SellItemButton.h"
#include "Components/Button.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/Image.h"
void UW_SellItemButton::NativeConstruct()
{
    Super::NativeConstruct();
    ItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    
    ItemButton->OnHovered.AddDynamic(this,&UW_SellItemButton::HoverItem);
    ItemButton->OnUnhovered.AddDynamic(this,&UW_SellItemButton::UnHoverItem);
    ItemButton->OnClicked.AddDynamic(this,&UW_SellItemButton::BuyItem);
}


void UW_SellItemButton::NativeDestruct()
{
    Super::NativeDestruct();

}
	
void UW_SellItemButton::InitButton(FItemData InitItemData)
{
    ItemData = InitItemData;
    if(ItemData.ItemIcon)
        ItemImage->SetBrushFromTexture(ItemData.ItemIcon);
}
	
void UW_SellItemButton::HoverItem()
{
    ItemSubsystem->HoverItem(ItemData);
}

	
void UW_SellItemButton::UnHoverItem()
{
    ItemSubsystem->UnHoverItem();
}

void UW_SellItemButton::BuyItem()
{
    ItemSubsystem->BuyItem(ItemData);
}