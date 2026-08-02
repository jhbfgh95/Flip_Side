// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopSellItemGrid.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/ShopItem/W_ShopItemSlot.h"



void UW_ShopSellItemGrid::NativeConstruct()
{
    Super::NativeConstruct();

    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

    ShopItemSubsystem->OnItemHovered.AddDynamic(this, &UW_ShopSellItemGrid::ShowDescrip);
    ShopItemSubsystem->OnItemUnHovered.AddDynamic(this, &UW_ShopSellItemGrid::HideDescrip);

    
    TArray<FItemData> SellItemData = ShopItemSubsystem->GetShopItemList();
    int32 SellItemCount = SellItemData.Num();
    
    for(int i =0; i<SellItemCount;i++)
    {

        UW_ShopItemSlot* ItemSlotWidget =Cast<UW_ShopItemSlot>(CreateWidget<UUserWidget>(GetWorld(), SellItemSlot));
        if (ItemSlotWidget)
        {
            SellItemSlots.Add(ItemSlotWidget);
            ItemSlotWidget->InitItemWidget(SellItemData[i]);
            SlotBox->AddChildToVerticalBox(ItemSlotWidget);
        }
    }

}

void UW_ShopSellItemGrid::NativeDestruct()
{
    ShopItemSubsystem->OnItemHovered.RemoveAll(this);
    ShopItemSubsystem->OnItemUnHovered.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopSellItemGrid::ShowDescrip(FItemData ItemInfo)
{
    //ItemBuyDescrip->SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopSellItemGrid::HideDescrip()
{
    //ItemBuyDescrip->SetVisibility(ESlateVisibility::Hidden);
}
