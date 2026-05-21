// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/FlipSideSoundUtils.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Kismet/GameplayStatics.h"
bool UShopItemWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel")) || MapName.Contains(TEXT("L_Tutorial_Shop_Level"))|| MapName.Contains(TEXT("L_Tutorial_TutoShop_Level"));
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

    MoneySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMoneyGISubsystem>();
    PlayerItemArray.Reset();
    for(int i =0; i <3 ;i++)
        PlayerItemArray.Add(DefaultSelecttemData);
}

void UShopItemWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    
    FString LevelName = UGameplayStatics::GetCurrentLevelName(&InWorld);
    // 레벨 이름으로 조건 검사
    if(LevelName.Equals(TEXT("L_ShopLevel"), ESearchCase::IgnoreCase))
    {
        for(int i =0; i <3 ;i++)
        {
            if(PlayerItemArray.IsValidIndex(i))
            {
                PlayerItemArray[i].ItemID = -1;
                PlayerItemArray[i].SameItemNum = 0;
            }
        }
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
	
void UShopItemWSubsystem::HoverPlayerItem(FItemData ItemData)
{
    OnPlayerItemHovered.Broadcast(ItemData);
}

void UShopItemWSubsystem::UnHoverPlayerItem()
{
    OnPlayerItemUnHovered.Broadcast();
}

void UShopItemWSubsystem::BuyItem(FItemData ItemData)
{
    int32 InvenIndex = GetSameItemInvenIndex(ItemData.ItemID);

    if(InvenIndex == -1)
    {
        int32 EmptyIvenNum = GetEmptyInvenIndex(ItemData.ItemID);
        if(EmptyIvenNum != -1)
        {
            if(MoneySubsystem->SpendMoney(EMoneyRecordType::Item, ItemData.Price))
            {
                FSelectItem SelectItemData;
                SelectItemData.ItemID = ItemData.ItemID;
                SelectItemData.SameItemNum = 1;
                PlayerItemArray[EmptyIvenNum] = SelectItemData;

                FFlipSideSoundUtils::PlayShopBuyClickSound(this);
                OnItemBuy.Broadcast(EmptyIvenNum);
            }
        }
    }
    else
    {
        if(MoneySubsystem->SpendMoney(EMoneyRecordType::Item, ItemData.Price))
        {
            PlayerItemArray[InvenIndex].SameItemNum++;
            FFlipSideSoundUtils::PlayShopBuyClickSound(this);
            OnItemBuy.Broadcast(InvenIndex);
        }
    }
}

int32 UShopItemWSubsystem::GetEmptyInvenIndex(int32 ItemID)
{
    for(int i =0; i<PlayerItemArray.Num() ; i++)
    {
        if(PlayerItemArray[i].ItemID == -1)
        {
            return i;
        }
    }

    return -1;
}

int32 UShopItemWSubsystem::GetSameItemInvenIndex(int32 FindItemID)
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


TArray<FItemData> UShopItemWSubsystem::GetShopItemList()
{
    return ShopItemArray;
}


void UShopItemWSubsystem::SellItem(FItemData ItemData)
{  
    for(int i = 0; i< PlayerItemArray.Num();i++)
    {
        if(PlayerItemArray[i].ItemID == ItemData.ItemID)
        {
            
            if(0<PlayerItemArray[i].SameItemNum)
            {
                PlayerItemArray[i].SameItemNum--;
                
            }

            if(PlayerItemArray[i].SameItemNum <=0)
            {
                PlayerItemArray[i].ItemID = -1;
                
                PlayerItemArray[i].SameItemNum= 0;
            }
            MoneySubsystem->AddSaleMoney(EMoneyRecordType::Item, ItemData.Price);
            FFlipSideSoundUtils::PlayShopSellClickSound(this);
            OnItemSell.Broadcast(i);
            return;
        }
        
    }
}
	
void UShopItemWSubsystem::ShopItemWarning(int32 WarningCode)
{
    OnShopItemWarning.Broadcast(WarningCode);
}
