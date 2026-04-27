// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemInven.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "UI/ShopItem/W_ShopInvenItemButton.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/TextBlock.h"
void UW_ShopItemInven::NativeConstruct()
{
    Super::NativeConstruct();

    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    
    DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    ShopItemSubsystem->OnItemBuy.AddDynamic(this, &UW_ShopItemInven::SetItem);
    ShopItemSubsystem->OnPlayerItemHovered.AddDynamic(this, &UW_ShopItemInven::ShowSellDes);
    ShopItemSubsystem->OnPlayerItemUnHovered.AddDynamic(this, &UW_ShopItemInven::HideSellDes);

    ItemIvens.Add(InvenItem1);
    ItemIvens.Add(InvenItem2);
    ItemIvens.Add(InvenItem3);
    FItemData DefaultsItemdata;
    DefaultsItemdata.ItemID = -1;
    ItemIvens[0]->SetItemData(DefaultsItemdata);
    ItemIvens[1]->SetItemData(DefaultsItemdata);
    ItemIvens[2]->SetItemData(DefaultsItemdata);
    HideSellDes();
}


void UW_ShopItemInven::NativeDestruct()
{
    ShopItemSubsystem->OnItemBuy.RemoveAll(this);
    ShopItemSubsystem->OnPlayerItemHovered.RemoveAll(this);
    ShopItemSubsystem->OnPlayerItemUnHovered.RemoveAll(this);
    Super::NativeDestruct();
}
	
void UW_ShopItemInven::ActiveInvenHandle()
{
	if(IsInvenOpen)
    {
        PlayAnimation(CloseAnim);
    }
    else
    {
        PlayAnimation(OpenAnim);
    }
}
	
void UW_ShopItemInven::SetItem(int32 Index)
{
    if(ItemIvens.Num() <Index)
        return;
    ItemIvens[Index]->SetCountText(ShopItemSubsystem->GetPlayerItem(Index).SameItemNum);
    FItemData ItemData;
    DataManagerSubsystem->TryGetItem(ShopItemSubsystem->GetPlayerItem(Index).ItemID, ItemData);
    ItemIvens[Index]->SetItemData(ItemData);
}
	
void UW_ShopItemInven::ShowSellDes(FItemData Itemdata)
{
    ItemSellDes->SetVisibility(ESlateVisibility::Visible);
}
	
	
void UW_ShopItemInven::HideSellDes()
{
    ItemSellDes->SetVisibility(ESlateVisibility::Hidden);
}