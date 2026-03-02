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
        TotalCoinCount++;
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum++;
        OnCoinCountUpdate.Broadcast(CurrentCoinSlotNum,ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum);
    }
}

void UShopCoinWSubsystem::DecreaseSlotCoinCount()
{
    if(CanDecreaseCoin(CurrentCoinSlotNum))
    {
        TotalCoinCount--;
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum--;
        OnCoinCountUpdate.Broadcast(CurrentCoinSlotNum,ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum);
    }
}


FCoinTypeStructure UShopCoinWSubsystem::GetCurrentSlotCoin()
{
    FCoinTypeStructure CoinInfo;
    CoinInfo = ShopCoinSlotArray[CurrentCoinSlotNum].CoinData;
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

    ShopCoinSlotArray[CurrentCoinSlotNum].CoinData = SetCoinInfo;
    ShopCoinSlotArray[CurrentCoinSlotNum].CoinClass = CoinClass;

    OnCoinCreated.Broadcast(CurrentCoinSlotNum, CoinClass);
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
        OnCoinSlotChange.Broadcast(true);
    }
}
//코인슬롯을 감소 시키는 방향으로 변경
void UShopCoinWSubsystem::ChangeCoinSlotLeft()
{
    if(0<=CurrentCoinSlotNum-1)
    {
        CurrentCoinSlotNum--;
        OnCoinSlotChange.Broadcast(false);
    }
}


void UShopCoinWSubsystem::ChangeCoinSlotByIndex(int32 SlotNum)
{
    if(SlotNum< ShopCoinSlotArray.Num())
    {
        CurrentCoinSlotNum = SlotNum;
        if(CurrentCoinSlotNum<SlotNum)
        {
            OnCoinSlotChange.Broadcast(true);
        }
        else
        {
            OnCoinSlotChange.Broadcast(false);
        }
        
    }
}

//현재 코인슬롯을 개방
void UShopCoinWSubsystem::UnlockCurrentCoinSlot()
{
    UE_LOG(LogTemp, Warning, TEXT("%d"), ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock);
    if(!ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock)
    {
        UE_LOG(LogTemp, Warning, TEXT("%d"), ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock);
        ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock = true;
        OnUnlockCoinSlot.Broadcast();
    }
    
    
}

bool UShopCoinWSubsystem::GetCurrentCoinUnlock()
{
    return ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock;
}


void UShopCoinWSubsystem::UnlockCoin()
{
    OnUnlockCoinSlot.Broadcast();
}