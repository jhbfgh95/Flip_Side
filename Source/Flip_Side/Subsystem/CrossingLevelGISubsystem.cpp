// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossingLevelGISubsystem.h"

#define COIN_SLOT_SIZE 10

void UCrossingLevelGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitCointSlot();

    GenerateTestCoin();
}

//코인 초기화
void UCrossingLevelGISubsystem::InitCointSlot()
{
    //디폴트 코인 값 생성
    DefaultCoin.BackWeaponID = -1;
    DefaultCoin.FrontWeaponID = -1;
    DefaultCoin.SameTypeCoinNum = -1;
    DefaultCoin.SlotNum = -1;

    for(int i =0; i< COIN_SLOT_SIZE;i++)
    {
        //코인 배열에 추가
        DefaultCoin.SlotNum = i;
        SlotCoinArray.Add(DefaultCoin);
    }
}

//코인슬롯에 코인 할당
void UCrossingLevelGISubsystem::SetSlotCoin(int SlotNum, FCoinTypeStructure CoinStruct)
{
    //슬롯 크기보다 작다면
    if(SlotNum < COIN_SLOT_SIZE)
    {
        //새로 넣는 코인의 슬롯값을 매개변수값으로 변경
        CoinStruct.SlotNum = SlotNum;

        //새로 셋팅할 코인슬롯이 배열 안에 있는 경우에만
        if(SlotNum<SlotCoinArray.Num())
        {
            SlotCoinArray[SlotNum] = CoinStruct;
            MakedCoinNum++;
        }
    }

}

//정해진 슬롯에 위치한 코인구조체를 반환
FCoinTypeStructure UCrossingLevelGISubsystem::GetSlotCoin(int SlotNum)
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

int32 UCrossingLevelGISubsystem::GetMakedCoinNum()
{
    return MakedCoinNum;
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
}
