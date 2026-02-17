// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameMode_Shop.h"
#include "Subsystem/ShopCardWSubsystem.h"
#include "Subsystem/ShopItemWSubsystem.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "Subsystem/CrossingLevelGISubsystem.h"


void AGameMode_Shop::ChangeBattleLevel()
{
    UCrossingLevelGISubsystem* CrossSubsystem = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>();
    UShopCoinWSubsystem* ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UShopCardWSubsystem* ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UShopItemWSubsystem* ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

    //코인값 넘겨줌
    for(int i =0; i<10; i++)
    {
        FCoinTypeStructure CoinData = ShopCoinSubsystem->GetSlotCoin(i);
        if(CoinData.FrontWeaponID != -1 && CoinData.BackWeaponID != -1)
            CrossSubsystem->SetSlotCoin(i, CoinData);
    }
    //카드값 넘겨줌
    for(int i =0; i<3; i++)
    {
        int32 CardID = ShopCardSubsystem->GetPlayerCardID(i);
        if(CardID != -1)
            CrossSubsystem->SetBattleCardID(i,CardID);
    }
    //아이템값 넘겨줌
    for(int i =0; i<ShopItemSubsystem->GetPlayerItemNum(); i++)
    {
        FSelectItem PlayerItem = ShopItemSubsystem->GetPlayerItem(i);
        if(PlayerItem.ItemID != -1 && PlayerItem.SameItemNum != -1)
        {
            CrossSubsystem->SetBattleUseItemID(i,PlayerItem.ItemID, PlayerItem.SameItemNum);
        }
    }
    
}


void AGameMode_Shop::SetCoinCreateMode()
{
    OnCoinCreateMode.Broadcast();
}

void AGameMode_Shop::SetShopMainMode()
{
    OnShopMainMode.Broadcast();
}

void AGameMode_Shop::SetCoinManageMode()
{
    OnCoinManageMode.Broadcast();
}


void AGameMode_Shop::SetShopItemMode()
{
    OnShopItemMode.Broadcast();
}

void AGameMode_Shop::SetCardSelectMode()
{
    OnSelectCardMode.Broadcast();
}


void AGameMode_Shop::SetUnlockWeaponMode()
{
    OnUnlockWeaponMode.Broadcast();
}