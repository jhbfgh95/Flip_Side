// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
bool UShopItemWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel")) || MapName.Contains(TEXT("L_ShopTutorialLevel"));
} 

//초기화
void UShopItemWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    DefaultItemData.ItemID = -1;

    DefaultSelecttemData.ItemID = -1;
    DefaultSelecttemData.SameItemNum = 0;

    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
    }

    if(DM)
    {
        DM->TryGetAllItems(ShopItemArray);
    }

}

void UShopItemWSubsystem::HoverItem(FItemData ItemData)
{
    OnItemHovered.Broadcast(ItemData);
}
void UShopItemWSubsystem::UnHoverItem()
{
    OnItemUnHovered.Broadcast();
}

void UShopItemWSubsystem::BuyItemByIndex(int32 Index)
{
    if(ShopItemArray.Num()<=Index) 
        return;

    int32 InvenIndex = GetPlayerInvenIndexByItemID(ShopItemArray[Index].ItemID);
    
    if(InvenIndex == -1)
    {
        FSelectItem AddItemData;
        AddItemData.ItemID = ShopItemArray[Index].ItemID;
        PlayerItemArray.Add(AddItemData);
    }
    else
    {
        PlayerItemArray[InvenIndex].SameItemNum++;
    }
    OnItemBuy.Broadcast(InvenIndex);
}

void UShopItemWSubsystem::BuyItem(FItemData ItemData)
{
    int32 InvenIndex = GetPlayerInvenIndexByItemID(ItemData.ItemID);
    
    if(InvenIndex == -1)
    {
        FSelectItem AddItemData;
        AddItemData.ItemID = ItemData.ItemID;
        PlayerItemArray.Add(AddItemData);

        PlayerItemArray[PlayerItemArray.Num()-1].SameItemNum++;

        OnItemBuy.Broadcast(PlayerItemArray.Num()-1);
    }
    else
    {
        PlayerItemArray[InvenIndex].SameItemNum++;
        OnItemBuy.Broadcast(InvenIndex);
    }
    
    
}

int32 UShopItemWSubsystem::GetPlayerInvenIndexByItemID(int32 FindItemID)
{
    if(FindItemID ==-1)
        return -1;

    for(int i=0; i< PlayerItemArray.Num();i++)
    {
        if(PlayerItemArray[i].ItemID == FindItemID)
        {
            return i;
        }
    }
    return -1;
}

FItemData UShopItemWSubsystem::GetItemDataByShopIndex(int32 ShopIndex)
{
    if(ShopIndex < ShopItemArray.Num())
    {
        return ShopItemArray[ShopIndex];
    }
    
    return DefaultItemData;//나중에 -1로 변경
}

int32 UShopItemWSubsystem::GetPlayerItemNum()
{
    return PlayerItemArray.Num();
}
FSelectItem UShopItemWSubsystem::GetPlayerItem(int32 index)
{
    if(index < PlayerItemArray.Num())
    {
        return PlayerItemArray[index];
    }
    return DefaultSelecttemData;
}