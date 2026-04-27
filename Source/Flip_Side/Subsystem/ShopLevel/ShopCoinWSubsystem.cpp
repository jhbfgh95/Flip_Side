// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

bool UShopCoinWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel")) || MapName.Contains(TEXT("L_Tutorial_Shop_Level"));
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

    IsCreateCoinFront = true;
}


bool UShopCoinWSubsystem::CanIncreaseCoin(int32 SlotNum)
{
    if(ShopCoinSlotArray.Num()-1<SlotNum)
        return false;

    if(ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID == -1)
    {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(MAX_TOTAL_COIN<=ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum)
     {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(MAX_TOTAL_COIN <= TotalCoinCount)
    {
        OnWarningCreate.Broadcast(4);
        return false;
    }
        

    

    return true;
}

bool UShopCoinWSubsystem::CanDecreaseCoin(int32 SlotNum)
{
    if(ShopCoinSlotArray.Num()-1<SlotNum)
        return false;

    if(ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID == -1)
    {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum<=0)
        return false;

    if(TotalCoinCount<=0)
        return false;
    
    return true;
}


int32 UShopCoinWSubsystem::GetSameWeaponInCoinSlot(int32 SlotNum, int32 WeaponID)
{
    if(ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID == -1 && ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID == -1)
        return -1;

    FCoinTypeStructure CheckCoinData;
    if(ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID == -1)
    {
        CheckCoinData.FrontWeaponID = WeaponID;
        CheckCoinData.BackWeaponID = ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID;
    }
    else
    {
        CheckCoinData.BackWeaponID = WeaponID;
        CheckCoinData.FrontWeaponID = ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID;
    }


    for(int i = 0; i < ShopCoinSlotArray.Num(); i++)
    {
        if(SlotNum != i)
        {
            if(ShopCoinSlotArray[i].CoinData.FrontWeaponID != -1 && ShopCoinSlotArray[i].CoinData.BackWeaponID != -1)
            {
                if(ShopCoinSlotArray[i].CoinData.FrontWeaponID == CheckCoinData.FrontWeaponID || ShopCoinSlotArray[i].CoinData.BackWeaponID == CheckCoinData.FrontWeaponID)
                {
                    if(ShopCoinSlotArray[i].CoinData.FrontWeaponID == CheckCoinData.BackWeaponID || ShopCoinSlotArray[i].CoinData.BackWeaponID == CheckCoinData.BackWeaponID)
                    {
                        ExecuteWarning(1);
                        return i;
                    }
                }
                    
            }
        }
        
    }
    return -1;
}

bool UShopCoinWSubsystem::IsTrySetSameWeapon(bool IsFront, int32 WeaponID)
{
    if(IsFront)
    {
        if(ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.BackWeaponID == WeaponID)
        {
            ExecuteWarning(0);
            return true;
        }
    }
    else
    {
        if(ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.FrontWeaponID == WeaponID)
        {
            ExecuteWarning(0);
            return true;
        }
    }
        
    return false;
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

void UShopCoinWSubsystem::IncreaseSlotCoinCount(int32 SlotNum)
{
    if(CanIncreaseCoin(SlotNum))
    {
        TotalCoinCount++;
        ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum++;
        OnCoinCountUpdate.Broadcast(SlotNum,ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum);
    }
}
void UShopCoinWSubsystem::DecreaseSlotCoinCount(int32 SlotNum)
{
    if(CanDecreaseCoin(SlotNum))
    {
        TotalCoinCount--;
        ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum--;
        OnCoinCountUpdate.Broadcast(SlotNum,ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum);
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
EWeaponClass UShopCoinWSubsystem::GetSlotCoinClass(int32 index)
{
    return ShopCoinSlotArray[index].CoinClass;
}

int32 UShopCoinWSubsystem::GetSlotCoinCount(int32 index)
{
    return ShopCoinSlotArray[index].CoinData.SameTypeCoinNum;
}


void UShopCoinWSubsystem::ResetCoin(int32 SlotNum)
{

    if(ShopCoinSlotArray.Num()<SlotNum)
        return;
    
    ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID = -1;
    ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID = -1;
    ShopCoinSlotArray[SlotNum].CoinClass = EWeaponClass::None;

    for(int i = ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;  0< i; i--)
    {
        DecreaseSlotCoinCount(SlotNum);
    }
    OnCoinCountUpdate.Broadcast(SlotNum, 0);
    OnSetWeapon.Broadcast(-1);
}
	
void UShopCoinWSubsystem::ResetCoinSide(int32 SlotNum, bool IsFront)
{
    if(IsFront)
    {
        ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID = -1;
    }
    else
    {
        ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID = -1;
    }

    for(int i = ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;  0< i; i--)
    {
        DecreaseSlotCoinCount(SlotNum);
    }
    OnCoinCountUpdate.Broadcast(SlotNum, 0);
    OnSetWeapon.Broadcast(-1);
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
    if(!ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock)
    {
        ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock = true;
        OnUnlockCoinSlot.Broadcast();
    }
    
}

bool UShopCoinWSubsystem::GetCurrentCoinUnlock()
{
    return ShopCoinSlotArray[CurrentCoinSlotNum].IsUnlock;
}

	
void UShopCoinWSubsystem::UnlockCoinSlot(int32 SlotNum)
{
    if(!ShopCoinSlotArray[SlotNum].IsUnlock)
    {
        ShopCoinSlotArray[SlotNum].IsUnlock = true;
        OnUnlockCoinSlot.Broadcast();
    }
}

void UShopCoinWSubsystem::UnlockCoin()
{
    OnUnlockCoinSlot.Broadcast();
}


int32 UShopCoinWSubsystem::GetCurrentCoinCount()
{
    return ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.SameTypeCoinNum;
}


void UShopCoinWSubsystem::ChangeSlotCoinSide(bool IsChangedSideFront)
{
    OnChangeSlotCoinSide.Broadcast(IsChangedSideFront);
}
	
int32 UShopCoinWSubsystem::GetCurrentSlotNum()
{
    return CurrentCoinSlotNum;
}


bool UShopCoinWSubsystem::GetCoinUnlockByIndex(int32 index)
{
    if(0<=index&&index<ShopCoinSlotArray.Num())
        return ShopCoinSlotArray[index].IsUnlock;
    else
        return false;
}
	
void UShopCoinWSubsystem::SelectCoin(int32 SlotNum)
{
    
    if(ShopCoinSlotArray.IsValidIndex(SlotNum))
        CurrentCoinSlotNum = SlotNum;

    OnCoinSlotChange.Broadcast();
    
}
	
void UShopCoinWSubsystem::ChangeCoinSide()
{
    if(IsCreateCoinFront)
        IsCreateCoinFront = false;
    else
        IsCreateCoinFront = true;
}

bool UShopCoinWSubsystem::GetIsCreateCoinFront()
{
    return IsCreateCoinFront;
}


void UShopCoinWSubsystem::SetWeaponToCoinSide(int32 WeaponID, EWeaponClass WeaponClass)
{
    if(IsTrySetSameWeapon(IsCreateCoinFront,WeaponID))
        return;
    
    if(IsCreateCoinFront)
    {
        if(ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.FrontWeaponID == -1)
        {
            if(GetSameWeaponInCoinSlot(CurrentCoinSlotNum, WeaponID) != -1)
                return;
        }
    }
    else
    {
        if(ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.BackWeaponID == -1)
        {
            if(GetSameWeaponInCoinSlot(CurrentCoinSlotNum, WeaponID) != -1)
                return;
        }
    }
    

    if(ShopCoinSlotArray[CurrentCoinSlotNum].CoinClass != WeaponClass)
    {
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinClass = WeaponClass;
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.FrontWeaponID = -1;
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.BackWeaponID = -1;
    }
    if(IsCreateCoinFront)
    {
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.FrontWeaponID = WeaponID;
    }
    else
    {
        ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.BackWeaponID = WeaponID;
    }
    OnSetWeapon.Broadcast(WeaponID);
}
	
int32 UShopCoinWSubsystem::GetCurrentCoinWeaponID(bool IsFront)
{
    if(IsFront)
    {
        return ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.FrontWeaponID;
    }
    else
    {
        return ShopCoinSlotArray[CurrentCoinSlotNum].CoinData.BackWeaponID;
    }
}

	
int32 UShopCoinWSubsystem::GetTotalCoinCount()
{
    return TotalCoinCount;
}
	
void UShopCoinWSubsystem::ExecuteWarning(int32 WarningCode)
{
    OnWarningCreate.Broadcast(WarningCode);
}
	
void UShopCoinWSubsystem::HoverWeapon(int32 WeaponID)
{
    OnHoverWeapon.Broadcast(WeaponID);
}
	

void UShopCoinWSubsystem::UnHoverWeapon()
{
    OnUnHoverWeapon.Broadcast();
}