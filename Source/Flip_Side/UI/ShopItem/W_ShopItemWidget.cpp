// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemWidget.h"

#include "UI/ShopItem/W_ShopItemDescription.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
void UW_ShopItemWidget::NativeConstruct()
{
    Super::NativeConstruct();
    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ShopItemSubsystem->OnItemHovered.AddDynamic(this, &UW_ShopItemWidget::SetDesText);

}
	
void UW_ShopItemWidget::NativeDestruct()
{
    
    Super::NativeDestruct();
}    
	
void UW_ShopItemWidget::SetDesText(FItemData ItemData)
{
    ItemDes->SetItemInfo(ItemData.ItemIcon, ItemData.ItemName ,ItemData.Item_DES);
}