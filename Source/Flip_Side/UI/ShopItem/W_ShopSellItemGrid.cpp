// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopSellItemGrid.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "UI/ShopItem/W_SellItemButton.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/TextBlock.h"


void UW_ShopSellItemGrid::NativeConstruct()
{
    Super::NativeConstruct();

    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

    ShopItemSubsystem->OnItemHovered.AddDynamic(this, &UW_ShopSellItemGrid::ShowDescrip);
    ShopItemSubsystem->OnItemUnHovered.AddDynamic(this, &UW_ShopSellItemGrid::HideDescrip);


    TArray<FItemData> SellItemData = ShopItemSubsystem->GetShopItemList();

    int32 SellItemCount = SellItemData.Num();

    
    for (int32 i = 0; i < SellItemCount; i++)
    {
        int32 Row = i / ColumnCount;
        int32 Col = i % ColumnCount;

        UW_SellItemButton* ItemButton =Cast<UW_SellItemButton>(CreateWidget<UUserWidget>(GetWorld(), SellItemButton));

        if (ItemButton && SellItemGrid)
        {
            ItemButton->InitButton(SellItemData[i]);
            SellItemGrid->AddChildToUniformGrid(ItemButton, Row, Col);
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
    ItemBuyDescrip->SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopSellItemGrid::HideDescrip()
{
    ItemBuyDescrip->SetVisibility(ESlateVisibility::Hidden);
}