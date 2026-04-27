// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemWidget.h"

#include "UI/ShopItem/W_ShopItemDescription.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
void UW_ShopItemWidget::NativeConstruct()
{
    Super::NativeConstruct();
    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ShopItemSubsystem->OnItemHovered.AddDynamic(this, &UW_ShopItemWidget::SetDesText);
    ShopItemSubsystem->OnItemUnHovered.AddDynamic(this, &UW_ShopItemWidget::HideDescrip);

    ShopItemSubsystem->OnPlayerItemHovered.AddDynamic(this, &UW_ShopItemWidget::SetDesText);
    ShopItemSubsystem->OnPlayerItemUnHovered.AddDynamic(this, &UW_ShopItemWidget::HideDescrip);

    HideDescrip();

}
	
void UW_ShopItemWidget::NativeDestruct()
{
    
    ShopItemSubsystem->OnItemHovered.RemoveAll(this);
    ShopItemSubsystem->OnItemUnHovered.RemoveAll(this);

    ShopItemSubsystem->OnPlayerItemHovered.RemoveAll(this);
    ShopItemSubsystem->OnPlayerItemUnHovered.RemoveAll(this);

    Super::NativeDestruct();
}    
	
void UW_ShopItemWidget::SetDesText(FItemData ItemData)
{
    ShowDescrip();
    ItemDes->SetItemInfo(ItemData.ItemName ,ItemData.Item_DES);
}
	
void UW_ShopItemWidget::ShowDescrip()
{
    ItemDes->SetVisibility(ESlateVisibility::Visible);
}
	
void UW_ShopItemWidget::HideDescrip()
{
    ItemDes->SetVisibility(ESlateVisibility::Hidden);
}