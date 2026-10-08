// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameMode_Shop.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/SaveGISubsystem.h"



void AGameMode_Shop::BeginPlay()
{
    Super::BeginPlay();

    if (USaveGISubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<USaveGISubsystem>())
    {
        SaveSubsystem->SaveCurrentGame();
    }
}

void AGameMode_Shop::ChangeBattleLevel()
{
    UCrossingLevelGISubsystem* CrossSubsystem = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>();
    UShopCoinWSubsystem* ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UShopCardWSubsystem* ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UShopItemWSubsystem* ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ULevelGISubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULevelGISubsystem>();

    const FString MapName = GetWorld()->GetName();
    if (MapName.Contains(TEXT("L_Tutorial_TutoShop_Level")) || MapName.Contains(TEXT("L_Tutorial_Shop_Level")))
    {
        if (!IsValid(CrossSubsystem) || !IsValid(ShopCoinSubsystem) || !IsValid(ShopCardSubsystem) ||
            !IsValid(ShopItemSubsystem) || !IsValid(LevelSubsystem)) return;

        // 빈 슬롯도 저장하여 이전 전투의 보유 데이터가 튜토리얼 보스전에 남지 않게 합니다.
        CrossSubsystem->SetIsCoinEmpty(ShopCoinSubsystem->GetIsCoinEmpty());
        for (int32 Slot = 0; Slot < 10; ++Slot)
            CrossSubsystem->SetSlotCoin(Slot, ShopCoinSubsystem->GetSlotCoin(Slot));
        for (int32 Slot = 0; Slot < 3; ++Slot)
        {
            CrossSubsystem->SetBattleCardID(ShopCardSubsystem->GetPlayerCardID(Slot), Slot);
            FSelectItem Item;
            if (Slot < ShopItemSubsystem->GetPlayerItemNum()) Item = ShopItemSubsystem->GetPlayerItem(Slot);
            const bool bHasItem = Item.ItemID >= 0 && Item.SameItemNum > 0;
            CrossSubsystem->SetBattleUseItemID(bHasItem ? Item.ItemID : -1, Slot, bHasItem ? Item.SameItemNum : 0);
        }
        LevelSubsystem->MovingTutorialLevel(1);
        return;
    }

    if(ShopCoinSubsystem->GetIsCoinEmpty())
    {
        CrossSubsystem->SetIsCoinEmpty(true);
    }
    else
    {
        CrossSubsystem->SetIsCoinEmpty(false);
        for(int i =0; i<10; i++)
        {
            FCoinTypeStructure CoinData = ShopCoinSubsystem->GetSlotCoin(i);
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


bool AGameMode_Shop::CheckHaveCoin()
{
    UShopCoinWSubsystem* ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    if(ShopCoinSubsystem->GetIsCoinEmpty())
    {
        return false;
    }
    return true;
}
