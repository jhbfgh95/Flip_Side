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
        CoinClassArray.Add(EWeaponClass::None);
    }
}


bool UShopCoinWSubsystem::CanIncreaseCoin(int32 SlotNum)
{
    if(CoinArray.Num()-1<SlotNum)
        return false;

    if(MAX_TOTAL_COIN<=CoinArray[SlotNum].SameTypeCoinNum)
        return false;

    if(MAX_TOTAL_COIN <= TotalCoinCount)
        return false;
    if(CoinArray[SlotNum].FrontWeaponID == -1 ||CoinArray[SlotNum].BackWeaponID == -1)
        return false;

    return true;
}

bool UShopCoinWSubsystem::CanDecreaseCoin(int32 SlotNum)
{
    if(CoinArray.Num()-1<SlotNum)
        return false;

    if(CoinArray[SlotNum].SameTypeCoinNum<=0)
        return false;

    if(TotalCoinCount<=0)
        return false;

    return true;
}

void UShopCoinWSubsystem::IncreaseSlotCoinCount(int32 SlotNum)
{
    if(CanIncreaseCoin(SlotNum))
    {
        UE_LOG(LogTemp, Warning, TEXT("증가 시작"));
        TotalCoinCount++;
        CoinArray[SlotNum].SameTypeCoinNum++;
    }
    UE_LOG(LogTemp, Warning, TEXT("증가 실행 : 총 개수 %d 슬롯 : %d, 슬롯 개수 : %d")
    ,TotalCoinCount, CoinArray[SlotNum].SlotNum, CoinArray[SlotNum].SameTypeCoinNum);
}

void UShopCoinWSubsystem::DecreaseSlotCoinCount(int32 SlotNum)
{
    if(CanDecreaseCoin(SlotNum))
    {
        TotalCoinCount--;
        CoinArray[SlotNum].SameTypeCoinNum--;
    }
    UE_LOG(LogTemp, Warning, TEXT("감소 실행 : 총 개수 %d 슬롯 : %d, 슬롯 개수 : %d")
    ,TotalCoinCount, CoinArray[SlotNum].SlotNum, CoinArray[SlotNum].SameTypeCoinNum);
}


FCoinTypeStructure UShopCoinWSubsystem::GetSlotCoin(int32 SlotNum)
{
    FCoinTypeStructure CoinInfo;

    if(CoinArray.Num()-1<SlotNum)
        return CoinInfo;

    CoinInfo = CoinArray[SlotNum];
    UE_LOG(LogTemp, Log,TEXT("선택한 코인 아이디 : %d"), CoinInfo.FrontWeaponID);
    return CoinInfo;
}


EWeaponClass UShopCoinWSubsystem::GetSlotCoinClass(int32 SlotNum)
{
    if(CoinClassArray.Num()<=SlotNum)
        return EWeaponClass::None;
    
    return CoinClassArray[SlotNum];
        
}

void UShopCoinWSubsystem::SetSlotCoin(FCoinTypeStructure SetCoinInfo, EWeaponClass CoinClass)
{
    if(CoinArray.Num()-1<SetCoinInfo.SlotNum)
        return;

    CoinArray[SetCoinInfo.SlotNum] = SetCoinInfo;
    CoinClassArray[SetCoinInfo.SlotNum] = CoinClass;
}


void UShopCoinWSubsystem::ResetCoin(int32 SlotNum)
{
    CoinArray[SlotNum].FrontWeaponID = -1;
    CoinArray[SlotNum].BackWeaponID = -1;
    CoinClassArray[SlotNum] = EWeaponClass::None;
    for(int i = CoinArray[SlotNum].SameTypeCoinNum;  0<= i; i--)
    {
        DecreaseSlotCoinCount(SlotNum);
    }
    
}
