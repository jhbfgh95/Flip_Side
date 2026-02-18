// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossingLevelGISubsystem.h"

#define COIN_SLOT_SIZE 10
#define CARD_SLOT_SIZE 3
#define USEABLE_SLOT_SIZE 3

void UCrossingLevelGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitSlots();

    GenerateTestCoin();
}

//코인 초기화
void UCrossingLevelGISubsystem::InitSlots()
{
    //디폴트 코인 값 생성
    DefaultCoin.BackWeaponID = -1;
    DefaultCoin.FrontWeaponID = -1;
    DefaultCoin.SameTypeCoinNum = -1;
    DefaultCoin.SlotNum = -1;

    DefaultItem.ItemID = -1;
    DefaultItem.SameItemNum = -1;

    for(int i =0; i< COIN_SLOT_SIZE;i++)
    {
        //코인 배열에 추가
        DefaultCoin.SlotNum = i;
        SlotCoinArray.Add(DefaultCoin);
    }

    //-1 3개 추가 -> 아이디가 -1은 없으니까 없는거로 판정
    for(int i = 0; i<CARD_SLOT_SIZE; i++)
    {
        CardIDArray.Add(-1);
    }

    //-1 3개 추가 -> 아이디가 -1은 없으니까 없는거로 판정
    for(int i = 0; i<USEABLE_SLOT_SIZE; i++)
    {
        SelectedUseableItemArray.Add(DefaultItem);
    }
}

//코인슬롯에 코인 할당
void UCrossingLevelGISubsystem::SetSlotCoin(int SlotNum, FCoinTypeStructure CoinStruct)
{
    if (SlotCoinArray.IsValidIndex(SlotNum))
    {
        CoinStruct.SlotNum = SlotNum;
        SlotCoinArray[SlotNum] = CoinStruct;
    }
}

void UCrossingLevelGISubsystem::SetBattleCardID(int32 CardID, int32 CardSlot)
{
    if(CardIDArray.IsValidIndex(CardSlot))
    {
        CardIDArray[CardSlot] = CardID;
    }
}

void UCrossingLevelGISubsystem::SetBattleUseItemID(int32 UseableItemID, int32 ItemSlot, int32 ItemNum)
{
    if(SelectedUseableItemArray.IsValidIndex(ItemSlot))
    {
        SelectedUseableItemArray[ItemSlot].ItemID = UseableItemID;
        SelectedUseableItemArray[ItemSlot].SameItemNum = ItemNum;
    }
}

/*
void UCrossingLevelGISubsystem::SetBattleUseItemID(FSelectItem UseItem)
{
    SelectedUseableItemArray.Empty();
    SelectedUseableItemArray.Add(UseItem);
}*/


//정해진 슬롯에 위치한 코인구조체를 반환
FCoinTypeStructure UCrossingLevelGISubsystem::GetSlotCoin(int SlotNum) const
{
    //슬롯 크기가 넘는 것을 불러온다면
    if(SlotNum > COIN_SLOT_SIZE )
    {
        //-1로 채워진 디폴트 코인을 반환
        return DefaultCoin;
    }

    //슬롯 크기에 해당하는 값이라면 해당 코인 구조체 반환
    return SlotCoinArray[SlotNum];
    
}

int32 UCrossingLevelGISubsystem::GetMakedCoinNum() const
{
    int32 Count = 0;
    for (const FCoinTypeStructure& Slot : SlotCoinArray)
    {
        if (Slot.FrontWeaponID != -1)
        {
            Count++;
        }
    }
    return Count;
}

TArray<int32> UCrossingLevelGISubsystem::GetBattleCardIDs()
{
    return CardIDArray;
}

FSelectItem UCrossingLevelGISubsystem::GetBattleUseItems(int SlotNum) const
{
    if(SlotNum > USEABLE_SLOT_SIZE)
    {
        return DefaultItem;
    }

    return SelectedUseableItemArray[SlotNum];
}

int32 UCrossingLevelGISubsystem::GetMakedItemNum() const
{
    int32 Count = 0;
    for (const FSelectItem& Item : SelectedUseableItemArray)
    {
        if (Item.ItemID != -1)
        {
            Count++;
        }
    }
    return Count;
}

//테스트용 코인 생성
void UCrossingLevelGISubsystem::GenerateTestCoin()
{
    FCoinTypeStructure SlotTestCoin;
    SlotTestCoin.SameTypeCoinNum = 3;

    for (int32 t = 0; t < 10; t++)
        {
            SlotTestCoin.FrontWeaponID = (t * 2) + 1;
            
            int32 BackID = (t * 2) + 2;
            SlotTestCoin.BackWeaponID = (BackID > 19) ? 19 : BackID;

            SetSlotCoin(t, SlotTestCoin);
        }

        //걍 id 1, 2, 3만 불러오게 설정
    for(int32 k = 0; k<3;k++)
    {
        //SetBattleCardID(k,k);
        SetBattleUseItemID(k+1,k,3);
    }
}
