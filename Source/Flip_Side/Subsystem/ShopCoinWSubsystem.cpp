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

    FShopCoinSlotData DefaultCoinSlot;

    
    for(int i =0; i< 10;i++)
    {
        ShopCoinSlotArray.Add(DefaultCoinSlot);
        //코인 배열에 추가
        DefaultCoin.SlotNum = i;
        ShopCoinSlotArray[i].CoinData = DefaultCoin;
        ShopCoinSlotArray[i].CoinClass = EWeaponClass::None;
    }
}


bool UShopCoinWSubsystem::CanIncreaseCoin(int32 SlotNum)
{
    if(ShopCoinSlotArray.Num()-1<SlotNum)
        return false;

    if(MAX_TOTAL_COIN<=ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum)
        return false;

    if(MAX_TOTAL_COIN <= TotalCoinCount)
        return false;
    if(ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID == -1)
        return false;

    return true;
}

bool UShopCoinWSubsystem::CanDecreaseCoin(int32 SlotNum)
{
    if(ShopCoinSlotArray.Num()-1<SlotNum)
        return false;

    if(ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum<=0)
        return false;

    if(TotalCoinCount<=0)
        return false;

    return true;
}

void UShopCoinWSubsystem::IncreaseSlotCoinCount()
{
    if(CanIncreaseCoin(CurrentCoinSlotNum))
    {
        UE_LOG(LogTemp, Warning, TEXT("증가 시작"));
        TotalCoinCount++;
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum++;
        OnCoinCountUpdate.Broadcast(ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum);
    }
    /*
    UE_LOG(LogTemp, Warning, TEXT("증가 실행 : 총 개수 %d 슬롯 : %d, 슬롯 개수 : %d")
    ,TotalCoinCount, CoinArray[SlotNum].SlotNum, CoinArray[SlotNum].SameTypeCoinNum);*/
}

void UShopCoinWSubsystem::DecreaseSlotCoinCount()
{
    if(CanDecreaseCoin(CurrentCoinSlotNum))
    {
        TotalCoinCount--;
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum--;
        OnCoinCountUpdate.Broadcast(ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum);
    }
    /*
    UE_LOG(LogTemp, Warning, TEXT("감소 실행 : 총 개수 %d 슬롯 : %d, 슬롯 개수 : %d")
    ,TotalCoinCount, CoinArray[SlotNum].SlotNum, CoinArray[SlotNum].SameTypeCoinNum);*/
}


FCoinTypeStructure UShopCoinWSubsystem::GetCurrentSlotCoin()
{
    FCoinTypeStructure CoinInfo;
    CoinInfo = ShopCoinSlotArray[CurrentCoinSlotNum].CoinData;
    UE_LOG(LogTemp, Log,TEXT("선택한 코인 아이디 : %d"), CoinInfo.FrontWeaponID);
    return CoinInfo;
}

EWeaponClass UShopCoinWSubsystem::GetCurrentSlotCoinClass()
{

    return ShopCoinSlotArray[CurrentCoinSlotNum].CoinClass;
        
}

void UShopCoinWSubsystem::SetSlotCoin(FCoinTypeStructure SetCoinInfo, EWeaponClass CoinClass)
{
    if(ShopCoinSlotArray.Num()-1<SetCoinInfo.SlotNum)
        return;

    ShopCoinSlotArray[SetCoinInfo.SlotNum].CoinData = SetCoinInfo;
    ShopCoinSlotArray[SetCoinInfo.SlotNum].CoinClass = CoinClass;
}


FCoinTypeStructure UShopCoinWSubsystem::GetSlotCoin(int32 index)
{
    if(ShopCoinSlotArray.Num()<index)
    {
        FCoinTypeStructure DefaultsCoinData;
        DefaultsCoinData.BackWeaponID = -1;
        DefaultsCoinData.FrontWeaponID = -1;
        return DefaultsCoinData;
    }
    else
    {
        return ShopCoinSlotArray[index].CoinData;
    }
}

void UShopCoinWSubsystem::ResetCoin(int32 SlotNum)
{
    ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID = -1;
    ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID = -1;
    ShopCoinSlotArray[SlotNum].CoinClass = EWeaponClass::None;
    for(int i = ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;  0<= i; i--)
    {
        //DecreaseSlotCoinCount(SlotNum);
    }
    
}

//코인슬롯을 증가
void UShopCoinWSubsystem::ChangeCoinSlotRight()
{
    if(CurrentCoinSlotNum+1<ShopCoinSlotArray.Num())
    {
        CurrentCoinSlotNum++;
        OnCoinSlotChange.Broadcast();
    }
}
//코인슬롯을 감소 시키는 방향으로 변경
void UShopCoinWSubsystem::ChangeCoinSlotLeft()
{
    if(0<CurrentCoinSlotNum-1)
    {
        CurrentCoinSlotNum--;
        OnCoinSlotChange.Broadcast();
    }
}


void UShopCoinWSubsystem::ChangeCoinSlotByIndex(int32 SlotNum)
{
    if(SlotNum< ShopCoinSlotArray.Num())
    {
        CurrentCoinSlotNum = SlotNum;
        OnCoinSlotChange.Broadcast();
    }
}

//현재 코인슬롯을 개방
void UShopCoinWSubsystem::UnlockCurrentCoinSlot()
{
    ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock = true;
    OnUnlockCoinSlot.Broadcast();
}

bool UShopCoinWSubsystem::GetCurrentCoinUnlock()
{
    return ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock;
}


void UShopCoinWSubsystem::UnlockCoin()
{
    OnUnlockCoinSlot.Broadcast();
}