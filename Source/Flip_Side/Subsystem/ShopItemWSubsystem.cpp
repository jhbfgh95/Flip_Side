// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopItemWSubsystem.h"

bool UShopItemWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel"));
} 

void UShopItemWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FSelectItem DefaultsItem;
    DefaultsItem.ItemID = -1;
    DefaultsItem.SameItemNum = 0;
    for(int i =0; i<3; i++)
        PlayerItemArray.Add(DefaultsItem);
}


void UShopItemWSubsystem::HoverShopItem(int32 ItemNum)
{
    OnItemHovered.Broadcast(ItemNum);
}
void UShopItemWSubsystem::UnHoverShopItem()
{
    OnItemUnHovered.Broadcast();
}
void UShopItemWSubsystem::HoverPlayerItem(int32 ItemNum)
{
    OnItemHovered.Broadcast(ItemNum);

}
void UShopItemWSubsystem::UnHoverPlayerItem(int32 ItemNum)
{

}


void UShopItemWSubsystem::AddItemByInven(int32 IndexNum)
{
    if(PlayerItemArray[IndexNum].ItemID == -1)
        return;

    if(MAX_ITEM_COUNT<= TotalItemCount)
        return;

    PlayerItemArray[IndexNum].SameItemNum++;
    TotalItemCount++;

    UE_LOG(LogTemp, Warning, TEXT("전체 개수 : %d, 인벤 %d의 개수 : %d, 인벤 %d의2개수 : %d, 인벤 %d의3 개수 : %d"),
    PlayerItemArray.Num(), 
    PlayerItemArray[0].ItemID,PlayerItemArray[0].SameItemNum,
    PlayerItemArray[1].ItemID,PlayerItemArray[1].SameItemNum,
    PlayerItemArray[2].ItemID,PlayerItemArray[2].SameItemNum);
}

void UShopItemWSubsystem::ReduceItemByInven(int32 IndexNum)
{
    if(PlayerItemArray[IndexNum].ItemID == -1)
        return;

    if(PlayerItemArray[IndexNum].SameItemNum<=0)
    {
        PlayerItemArray[IndexNum].ItemID = -1;
        return;
    }
    if(TotalItemCount <= 0)
        return;
    
        

    PlayerItemArray[IndexNum].SameItemNum--;
    TotalItemCount--;

        UE_LOG(LogTemp, Warning, TEXT("전체 개수 : %d, 인벤 %d의 개수 : %d, 인벤 %d의2개수 : %d, 인벤 %d의3 개수 : %d"),
    PlayerItemArray.Num(), 
    PlayerItemArray[0].ItemID,PlayerItemArray[0].SameItemNum,
    PlayerItemArray[1].ItemID,PlayerItemArray[1].SameItemNum,
    PlayerItemArray[2].ItemID,PlayerItemArray[2].SameItemNum);
}

void UShopItemWSubsystem::AddItem(int32 GetItemID)
{
    int32 ItemSlot =0;
    ItemSlot =GetCanAddItemIndex(GetItemID);
    UE_LOG(LogTemp, Warning, TEXT("전체 개수 : %d"),ItemSlot);
    if(ItemSlot == -1)
    {
        return ;
    }

    if(PlayerItemArray[ItemSlot].ItemID != GetItemID)
    {
        PlayerItemArray[ItemSlot].ItemID = GetItemID;
        
    }

    PlayerItemArray[ItemSlot].SameItemNum++;
    TotalItemCount++;
    UE_LOG(LogTemp, Warning, TEXT("전체 개수 : %d, 인벤 %d의 개수 : %d, 인벤 %d의2개수 : %d, 인벤 %d의3 개수 : %d"),
    PlayerItemArray.Num(), 
    PlayerItemArray[0].ItemID,PlayerItemArray[0].SameItemNum,
    PlayerItemArray[1].ItemID,PlayerItemArray[1].SameItemNum,
    PlayerItemArray[2].ItemID,PlayerItemArray[2].SameItemNum);
    
}

void UShopItemWSubsystem::ReduceItem(int32 GetItemID)
{
    int32 ItemSlot =0;
    ItemSlot =GetCanReduceItemIndex(GetItemID);

    if(ItemSlot == -1)
    {
        return ;
    }

    if(PlayerItemArray[ItemSlot].ItemID == GetItemID)
    {
        PlayerItemArray[ItemSlot].SameItemNum--;
        TotalItemCount--;
    }

    
    UE_LOG(LogTemp, Warning, TEXT("전체 개수 : %d, 인벤 %d의 개수 : %d, 인벤 %d의2개수 : %d, 인벤 %d의3 개수 : %d"),
    PlayerItemArray.Num(), 
    PlayerItemArray[0].ItemID,PlayerItemArray[0].SameItemNum,
    PlayerItemArray[1].ItemID,PlayerItemArray[1].SameItemNum,
    PlayerItemArray[2].ItemID,PlayerItemArray[2].SameItemNum);
}

int32 UShopItemWSubsystem::GetCanReduceItemIndex(int32 GetItemID)
{
    //1. 개수가 늘어날 수 있는지?
    if(TotalItemCount <= 0)
    {
        return -1;
    }
    //2.종류가 3종류미만 인가?
    for(int i =0; i < PlayerItemArray.Num(); i++)
    {
        if(PlayerItemArray[i].ItemID == GetItemID)
        {
            if(0<PlayerItemArray[i].SameItemNum)
            {
                return i;
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}

int32 UShopItemWSubsystem::GetCanAddItemIndex(int32 GetItemID)
{
    //1. 개수가 늘어날 수 있는지?
    if(MAX_ITEM_COUNT <= TotalItemCount)
    {
        return -1;
    }
    //2.종류가 3종류미만 인가?
    for(int i =0; i < PlayerItemArray.Num(); i++)
    {
        if(PlayerItemArray[i].ItemID == -1)
        {
            return i;
        }
        if(PlayerItemArray[i].ItemID == GetItemID)
        {
            return i;
        }
    }
    return -1;
}