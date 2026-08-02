// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopPlayerItemGrid.h"
#include "UI/ShopItem/W_ShopPlayerItemSlot.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopItem/W_ShopItemSlot.h"

void UW_ShopPlayerItemGrid::NativeConstruct()
{
    Super::NativeConstruct();
    
    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

    ShopItemSubsystem->OnItemHovered.AddDynamic(this, &UW_ShopPlayerItemGrid::ShowDescrip);
    ShopItemSubsystem->OnItemUnHovered.AddDynamic(this, &UW_ShopPlayerItemGrid::HideDescrip);

    
    for(int i =0; i<3;i++)
    {
        UW_ShopPlayerItemSlot* ItemSlotWidget =Cast<UW_ShopPlayerItemSlot>(CreateWidget<UUserWidget>(GetWorld(), PlayerItemSlot));
        if (ItemSlotWidget)
        {
            PlayerItemSlots.Add(ItemSlotWidget);
            UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(ItemSlotWidget);
            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

            ItemSlotWidget->InitItemWidget(i);
        }
    }
}

void UW_ShopPlayerItemGrid::NativeDestruct()
{
    ShopItemSubsystem->OnItemHovered.RemoveAll(this);
    ShopItemSubsystem->OnItemUnHovered.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopPlayerItemGrid::ShowDescrip(FItemData ItemInfo)
{
    //ItemBuyDescrip->SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopPlayerItemGrid::HideDescrip()
{
    //ItemBuyDescrip->SetVisibility(ESlateVisibility::Hidden);
}
