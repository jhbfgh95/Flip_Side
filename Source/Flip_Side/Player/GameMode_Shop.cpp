// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameMode_Shop.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/LevelGISubsystem.h"

void AGameMode_Shop::ChangeBattleLevel()
{
    UCrossingLevelGISubsystem* CrossSubsystem = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>();
    UShopCoinWSubsystem* ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UShopCardWSubsystem* ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UShopItemWSubsystem* ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ULevelGISubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULevelGISubsystem>();


    if(ShopCoinSubsystem->GetIsCoinEmpty())
    {
        CrossSubsystem->SetIsCoinEmpty(true);
    }
    else
    {
        CrossSubsystem->SetIsCoinEmpty(false);
        //코인값 넘겨줌
        for(int i =0; i<10; i++)
        {
            FCoinTypeStructure CoinData = ShopCoinSubsystem->GetSlotCoin(i);
            
            UE_LOG(LogTemp, Warning, TEXT("앞면 정보 %d / 뒷면정보%d"), CoinData.FrontWeaponID,CoinData.BackWeaponID);

            //if(CoinData.FrontWeaponID != -1 && CoinData.BackWeaponID != -1)
            CrossSubsystem->SetSlotCoin(i, CoinData);
        }
    }
    //카드값 넘겨줌
    for(int i =0; i<3; i++)
    {
        int32 CardID = ShopCardSubsystem->GetPlayerCardID(i);
        CrossSubsystem->SetBattleCardID(CardID,i);
    }
    //아이템값 넘겨줌
    for(int i =0; i<ShopItemSubsystem->GetPlayerItemNum(); i++)
    {
        FSelectItem PlayerItem = ShopItemSubsystem->GetPlayerItem(i);
        if(PlayerItem.ItemID != -1 && PlayerItem.SameItemNum != -1)
        {
            CrossSubsystem->SetBattleUseItemID(PlayerItem.ItemID,i, PlayerItem.SameItemNum);
        }
    }

    LevelSubsystem->MoveBattleLevel();
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


void AGameMode_Shop::SetCheckBossMode()
{
    OnCheckBossMode.Broadcast();
}