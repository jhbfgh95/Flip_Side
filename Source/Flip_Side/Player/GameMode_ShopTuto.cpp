// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameMode_ShopTuto.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"

void AGameMode_ShopTuto::ChangeBattleLevel()
{
    UCrossingLevelGISubsystem* CrossSubsystem = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>();
    UShopCoinWSubsystem* ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UShopCardWSubsystem* ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UShopItemWSubsystem* ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ULevelGISubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULevelGISubsystem>();


    if (!IsValid(CrossSubsystem) || !IsValid(ShopCoinSubsystem) || !IsValid(ShopCardSubsystem) ||
        !IsValid(ShopItemSubsystem) || !IsValid(LevelSubsystem)) return;

    CrossSubsystem->SetIsCoinEmpty(ShopCoinSubsystem->GetIsCoinEmpty());
    // 빈 슬롯도 전달하여 이전 보유 데이터를 덮어씁니다.
    for (int32 Slot = 0; Slot < 10; ++Slot)
        CrossSubsystem->SetSlotCoin(Slot, ShopCoinSubsystem->GetSlotCoin(Slot));
    //카드값 넘겨줌
    for(int i =0; i<3; i++)
    {
        int32 CardID = ShopCardSubsystem->GetPlayerCardID(i);
        CrossSubsystem->SetBattleCardID(CardID,i);
    }
    //아이템값 넘겨줌
    for(int i =0; i<3; i++)
    {
        FSelectItem PlayerItem;
        if (i < ShopItemSubsystem->GetPlayerItemNum()) PlayerItem = ShopItemSubsystem->GetPlayerItem(i);
        const bool bHasItem = PlayerItem.ItemID >= 0 && PlayerItem.SameItemNum > 0;
        CrossSubsystem->SetBattleUseItemID(bHasItem ? PlayerItem.ItemID : -1, i, bHasItem ? PlayerItem.SameItemNum : 0);
    }
    LevelSubsystem->MovingTutorialLevel(1);
}
