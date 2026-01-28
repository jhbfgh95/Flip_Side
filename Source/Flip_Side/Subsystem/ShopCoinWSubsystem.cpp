// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopCoinWSubsystem.h"

bool UShopCoinWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UShopCoinWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FCoinTypeStructure DefaultCoin;
    DefaultCoin.BackWeaponID = -1;
    DefaultCoin.FrontWeaponID = -1;
    DefaultCoin.SameTypeCoinNum = 0;
    DefaultCoin.SlotNum = -1;

    for(int i =0; i< 10;i++)
    {
        //코인 배열에 추가
        DefaultCoin.SlotNum = i;
        CoinArray.Add(DefaultCoin);
    }
}


bool UShopCoinWSubsystem::CanAddCoin(int32 SlotNum)
{
    if(CoinArray.Num()-1<SlotNum)
        return false;

    if(MAX_TOTAL_COIN<=CoinArray[SlotNum].SameTypeCoinNum)
        return false;

    if(MAX_TOTAL_COIN <= TotalCoinCount)
        return false;
        

    return true;
}

bool UShopCoinWSubsystem::CanRemoveCoin(int32 SlotNum)
{
    if(CoinArray.Num()-1<SlotNum)
        return false;

    if(CoinArray[SlotNum].SameTypeCoinNum<=0)
        return false;

    if(TotalCoinCount<=0)
        return false;

    return true;
}

void UShopCoinWSubsystem::AddSlotCoin(int32 SlotNum)
{
    if(CanAddCoin(SlotNum))
    {
        TotalCoinCount++;
        CoinArray[SlotNum].SameTypeCoinNum++;
    }
    
}

void UShopCoinWSubsystem::RemoveSlotCoin(int32 SlotNum)
{
    if(CanRemoveCoin(SlotNum))
    {
        TotalCoinCount--;
        CoinArray[SlotNum].SameTypeCoinNum--;
    }
}


FCoinTypeStructure UShopCoinWSubsystem::GetSlotCoin(int32 SlotNum)
{
    FCoinTypeStructure CoinInfo;
    if(CoinArray.Num()-1<SlotNum)
        return CoinInfo;
    CoinInfo = CoinArray[SlotNum];
    
    return CoinInfo;

}

void UShopCoinWSubsystem::SetSlotCoin(FCoinTypeStructure SetCoinInfo)
{
    if(CoinArray.Num()-1<SetCoinInfo.SlotNum)
        return;

    CoinArray[SetCoinInfo.SlotNum] = SetCoinInfo;
}