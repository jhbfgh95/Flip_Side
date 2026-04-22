// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopInvenItemButton.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
void UW_ShopInvenItemButton::NativeConstruct()
{
    Super::NativeConstruct();
    ItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ItemButton->OnHovered.AddDynamic(this, &UW_ShopInvenItemButton::HoverItem);
    SetItemImage(nullptr);
}

void UW_ShopInvenItemButton::NativeDestruct()
{

    Super::NativeDestruct();
}
	
void UW_ShopInvenItemButton::HoverItem()
{
    ItemSubsystem->HoverItem(CurrentItemData);
}

void UW_ShopInvenItemButton::SetItemData(FItemData ItemData)
{
    CurrentItemData = ItemData;
    SetItemImage(CurrentItemData.ItemIcon);
}
	
void UW_ShopInvenItemButton::SetCountText(int32 Count)
{
    ItemCountText->SetText(FText::AsNumber(Count));
}
	
void UW_ShopInvenItemButton::SetItemImage(UTexture2D* Texture)
{
    if(Texture)
    {
        ItemImage->SetColorAndOpacity(FLinearColor(1,1,1,1));
        ItemImage->SetBrushFromTexture(Texture);
    }
    else
    {
        ItemImage->SetColorAndOpacity(FLinearColor(1,1,1,0));
    }

}