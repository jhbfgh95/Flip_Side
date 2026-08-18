// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
bool UShopCoinWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel")) || MapName.Contains(TEXT("L_Tutorial_Shop_Level"))|| MapName.Contains(TEXT("L_Tutorial_TutoShop_Level"));
} 

void UShopCoinWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    MoneySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMoneyGISubsystem>();
    CrossLevelSubsystem =GetWorld()->GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    TotalCoinCount = 0;

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
    }

}


void UShopCoinWSubsystem::InitCoinSetting()
{
    //CrossLevelSubsystem->
    //추후 레벨 넘어가면
}


bool UShopCoinWSubsystem::CanIncreaseCoin(int32 SlotIndex,int32 Amount)
{
    if(!ShopCoinSlotArray.IsValidIndex(SlotIndex) || Amount <= 0)
        return false;

    if(ShopCoinSlotArray[SlotIndex].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotIndex].CoinData.BackWeaponID == -1)
    {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(MAX_TOTAL_COIN < ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum + Amount)
    {
        OnWarningCreate.Broadcast(4);
        return false;
    }

    if(MAX_TOTAL_COIN < TotalCoinCount + Amount)
    {
        OnWarningCreate.Broadcast(4);
        return false;
    }

    return true;
}

bool UShopCoinWSubsystem::CanDecreaseCoin(int32 SlotIndex, int32 Amount)
{
    if(!ShopCoinSlotArray.IsValidIndex(SlotIndex) || Amount <= 0)
        return false;

    if(ShopCoinSlotArray[SlotIndex].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotIndex].CoinData.BackWeaponID == -1)
    {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum - Amount < 0)
        return false;

    if(TotalCoinCount<=0)
        return false;
    
    return true;
}

bool UShopCoinWSubsystem::BuyCoinSlot(int32 BuySlotLevel)
{
    if(!ShopCoinSlotArray.IsValidIndex(LastUnlockCoinSlotIndex+1))
        return false;

    FCoinTypeStructure CoinType;
    CoinType.Level = BuySlotLevel;
    int32 SlotBuyPrice = 0;
    int32 SlotHP;

    if(!DataManager->GetCoinSlotLevelStats(CoinType, SlotBuyPrice, SlotHP))
    {
        return false;
    }
    
    if(MoneySubsystem->SpendMoney(EMoneyRecordType::CoinSlot, SlotBuyPrice))
    {
        LastUnlockCoinSlotIndex += 1;
        
        ShopCoinSlotArray[LastUnlockCoinSlotIndex].IsUnlock = true;
        
        ShopCoinSlotArray[LastUnlockCoinSlotIndex].CoinData.Level = CoinType.Level;

        return true;
    }

    return false;
}
	
bool UShopCoinWSubsystem::SellCoinSlot(int32 BuySlotLevel)
{
    if(!ShopCoinSlotArray.IsValidIndex(LastUnlockCoinSlotIndex-1))
        return false;

    FCoinTypeStructure CoinType;
    CoinType.Level = BuySlotLevel;
    int32 SlotSellPrice = 0;
    int32 SlotHP;

    if(!DataManager->GetCoinSlotLevelStats(CoinType, SlotSellPrice, SlotHP))
    {
        return false;
    }

    MoneySubsystem->AddSaleMoney(EMoneyRecordType::CoinSlot, SlotSellPrice);

    ShopCoinSlotArray[LastUnlockCoinSlotIndex].IsUnlock = false;

    LastUnlockCoinSlotIndex-=1;

    return true;
}

void UShopCoinWSubsystem::IncreaseCoinSlotCoin(int32 SlotIndex, int32 Amount)
{
    if(CanIncreaseCoin(SlotIndex,Amount))
    {
        if(MoneySubsystem->SpendMoney(EMoneyRecordType::Coin, 50*Amount))
        {
            TotalCoinCount+=Amount;
            ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum+=Amount;
        }
    }
}


void UShopCoinWSubsystem::DecreaseCoinSlotCoin(int32 SlotIndex, int32 Amount)
{
    if(CanDecreaseCoin(SlotIndex, Amount))
    {
        MoneySubsystem->AddSaleMoney(EMoneyRecordType::Coin, 50*Amount);
        TotalCoinCount-=Amount;
        ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum-=Amount;
    }
}

int32 UShopCoinWSubsystem::GetUnlockCoinSlotCount()
{   
    int32 SlotCount = 0;

    for(int i =0; i< ShopCoinSlotArray.Num(); i++)
    {
        if(ShopCoinSlotArray[i].IsUnlock)
            SlotCount++;
        else
            break;
    }
    
    return SlotCount;
}

bool UShopCoinWSubsystem::GetCoinSlotUnlock(int32 Index)
{
    return ShopCoinSlotArray.IsValidIndex(Index) && ShopCoinSlotArray[Index].IsUnlock;
}

FCoinTypeStructure UShopCoinWSubsystem::GetCoinSlotCoinType(int32 SlotIndex)
{
    FCoinTypeStructure EmptyCoinType;

    if(!ShopCoinSlotArray.IsValidIndex(SlotIndex))
    {
        return EmptyCoinType;
    }
        
    
    return ShopCoinSlotArray[SlotIndex].CoinData;
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
                if(ShopCoinSlotArray[i].CoinData.FrontWeaponID == CheckCoinData.FrontWeaponID)
                {
                    if(ShopCoinSlotArray[i].CoinData.BackWeaponID == CheckCoinData.BackWeaponID)
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
    /*
    if(IsFront)
    {
        if(ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.BackWeaponID == WeaponID)
        {
            ExecuteWarning(0);
            return true;
        }
    }
    else
    {
        if(ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.FrontWeaponID == WeaponID)
        {
            ExecuteWarning(0);
            return true;
        }
    }*/
        
    return false;
}

FCoinTypeStructure UShopCoinWSubsystem::GetSlotCoin(int32 index)
{
    if(!ShopCoinSlotArray.IsValidIndex(index))
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


void UShopCoinWSubsystem::ResetCoinSlot(int32 SlotIndex)
{
    if(!ShopCoinSlotArray.IsValidIndex(SlotIndex))
        return;
    
    ShopCoinSlotArray[SlotIndex].CoinData.FrontWeaponID = -1;
    ShopCoinSlotArray[SlotIndex].CoinData.BackWeaponID = -1;

    const int32 CoinCount = ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum;

    TotalCoinCount -= CoinCount;
    ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum = 0;

    MoneySubsystem->AddSaleMoney(EMoneyRecordType::Coin, 50, CoinCount);

}
	
bool UShopCoinWSubsystem::IncreaseCoinSlot(int32 SlotNum)
{
    if(!ShopCoinSlotArray.IsValidIndex(SlotNum))
        return false;

    if(MoneySubsystem->SpendMoney(EMoneyRecordType::CoinSlot, 100))
    {
        
        ShopCoinSlotArray[SlotNum].IsUnlock = true;
        LastUnlockCoinSlotIndex = FMath::Max(LastUnlockCoinSlotIndex, SlotNum);
        return true;
    }

    return false;
}

bool UShopCoinWSubsystem::DecreaseCoinSlot(int32 SlotNum)
{
    if(!ShopCoinSlotArray.IsValidIndex(SlotNum))
        return false;

    ShopCoinSlotArray[SlotNum].IsUnlock = false;

    ShopCoinSlotArray[SlotNum].CoinData.FrontWeaponID =-1;
    ShopCoinSlotArray[SlotNum].CoinData.BackWeaponID =-1;

    int CoinCount = ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;

    TotalCoinCount -= ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;

    ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum = 0;

    MoneySubsystem->AddSaleMoney(EMoneyRecordType::Coin, 50, CoinCount);
    MoneySubsystem->AddSaleMoney(EMoneyRecordType::CoinSlot, 100);

    if(SlotNum == LastUnlockCoinSlotIndex)
        --LastUnlockCoinSlotIndex;
    return true;
}


void UShopCoinWSubsystem::SetCoinSlotFrontWeapon(int32 SlotIndex, int32 WeaponID)
{
    ShopCoinSlotArray[SlotIndex].CoinData.FrontWeaponID = WeaponID;
}

void UShopCoinWSubsystem::SetCoinSlotBackWeapon(int32 SlotIndex, int32 WeaponID)
{
    ShopCoinSlotArray[SlotIndex].CoinData.BackWeaponID = WeaponID;
}

	
int32 UShopCoinWSubsystem::GetTotalCoinCount()
{
    return TotalCoinCount;
}
	
void UShopCoinWSubsystem::ExecuteWarning(int32 WarningCode)
{
    OnWarningCreate.Broadcast(WarningCode);
}
	
bool UShopCoinWSubsystem::GetIsCoinSlotUnlockByIndex(int32 index)
{
    return GetCoinSlotUnlock(index);
}

void UShopCoinWSubsystem::ChangeCoinSlotOrder()
{
}


int32 UShopCoinWSubsystem::GetUnlockSlotLastIndex()
{
    return LastUnlockCoinSlotIndex;
}

bool UShopCoinWSubsystem::GetIsCoinEmpty()
{
    if(TotalCoinCount <= 0)
    {
        return true;
    }
    else
        return false;
}