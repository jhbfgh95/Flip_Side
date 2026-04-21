// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopSellItemGrid.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "UI/ShopItem/W_SellItemButton.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"



void UW_ShopSellItemGrid::NativeConstruct()
{
    Super::NativeConstruct();

    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
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
    Super::NativeDestruct();
}
