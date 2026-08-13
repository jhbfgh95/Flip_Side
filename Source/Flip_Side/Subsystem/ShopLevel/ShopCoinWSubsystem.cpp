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

    IsCreateCoinFront = true;
}


void UShopCoinWSubsystem::InitCoinSetting()
{
    //CrossLevelSubsystem->
    //추후 레벨 넘어가면
}


bool UShopCoinWSubsystem::CanIncreaseCoin(int32 SlotIndex,int32 Amount)
{
    if(ShopCoinSlotArray.IsValidIndex(SlotIndex))
        return false;

    if(ShopCoinSlotArray[SlotIndex].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotIndex].CoinData.BackWeaponID == -1)
    {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(MAX_TOTAL_COIN<=ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum+Amount)
    {
        OnWarningCreate.Broadcast(4);
        return false;
    }

    if(MAX_TOTAL_COIN <= TotalCoinCount)
    {
        OnWarningCreate.Broadcast(4);
        return false;
    }

    return true;
}

bool UShopCoinWSubsystem::CanDecreaseCoin(int32 SlotIndex, int32 Amount)
{
    if(ShopCoinSlotArray.IsValidIndex(SlotIndex))
        return false;

    if(ShopCoinSlotArray[SlotIndex].CoinData.FrontWeaponID == -1 ||ShopCoinSlotArray[SlotIndex].CoinData.BackWeaponID == -1)
    {
        OnWarningCreate.Broadcast(3);
        return false;
    }

    if(ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum-Amount<=0)
        return false;

    if(TotalCoinCount<=0)
        return false;
    
    return true;
}

bool UShopCoinWSubsystem::BuyCoinSlot(int32 BuySlotLevel)
{
    if(!ShopCoinSlotArray.IsValidIndex(CurrentCoinSlotIndex+1))
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
        
        CurrentCoinSlotIndex = LastUnlockCoinSlotIndex;

        SelectCoin(CurrentCoinSlotIndex);

        OnChangeCoinSlotCount.Broadcast(true);

        return true;
    }

    return false;
}
	
bool UShopCoinWSubsystem::SellCoinSlot(int32 BuySlotLevel)
{
    if(!ShopCoinSlotArray.IsValidIndex(CurrentCoinSlotIndex+1))
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

    LastUnlockCoinSlotIndex-=1;
    
    ShopCoinSlotArray[CurrentCoinSlotIndex].IsUnlock = false;

    if(CurrentCoinSlotIndex == LastUnlockCoinSlotIndex)
    {
        CurrentCoinSlotIndex -=1;
        SelectCoin(CurrentCoinSlotIndex);
    }
        
    OnChangeCoinSlotCount.Broadcast(false);
    OnCoinSlotChange.Broadcast();

    return true;
}

void UShopCoinWSubsystem::IncreaseCoinSlotCoin(int32 SlotIndex, int32 Amount)
{
    if(CanIncreaseCoin(SlotIndex,Amount))
    {
        if(MoneySubsystem->SpendMoney(EMoneyRecordType::Coin, 50*Amount))
        {
            TotalCoinCount+=Amount;
            ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.SameTypeCoinNum+=Amount;
            OnCoinCountUpdate.Broadcast(CurrentCoinSlotIndex,ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.SameTypeCoinNum);
        }
    }
}


void UShopCoinWSubsystem::DecreaseCoinSlotCoin(int32 SlotIndex, int32 Amount)
{
    if(CanDecreaseCoin(SlotIndex, Amount))
    {
        MoneySubsystem->AddSaleMoney(EMoneyRecordType::Coin, 50*Amount);
        TotalCoinCount-=Amount;
        ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.SameTypeCoinNum-=Amount;
        OnCoinCountUpdate.Broadcast(CurrentCoinSlotIndex,ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.SameTypeCoinNum);

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

int32 UShopCoinWSubsystem::GetCurrentCoinSlotIndex()
{
    return CurrentCoinSlotIndex;
}

int32 UShopCoinWSubsystem::GetCoinSlotCoinCount(int32 SlotIndex)
{
    if(ShopCoinSlotArray.IsValidIndex(SlotIndex))
        return -1;
    
    return ShopCoinSlotArray[SlotIndex].CoinData.SameTypeCoinNum;
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
    }
        
    return false;
}

FCoinTypeStructure UShopCoinWSubsystem::GetCurrentSlotCoin()
{
    FCoinTypeStructure CoinInfo;
    CoinInfo.FrontWeaponID = -1;
    CoinInfo.BackWeaponID = -1;
    CoinInfo.SameTypeCoinNum = -1;
    CoinInfo.SlotNum = -1;
    
    if(ShopCoinSlotArray.IsValidIndex(CurrentCoinSlotIndex))
        CoinInfo = ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData;

    
    UE_LOG(LogTemp, Warning, TEXT("초기 설정 코인슬롯 값 앞 %d뒤%d "), CoinInfo.FrontWeaponID, CoinInfo.BackWeaponID);

    return CoinInfo;
}

void UShopCoinWSubsystem::SetSlotCoin(FCoinTypeStructure SetCoinInfo, EWeaponClass CoinClass)
{
    if(ShopCoinSlotArray.Num()-1<SetCoinInfo.SlotNum)
        return;

    ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData = SetCoinInfo;

    OnCoinCreated.Broadcast(CurrentCoinSlotIndex, CoinClass);
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

    int CoinCount = ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;

    TotalCoinCount -= CoinCount;
    ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum = 0;

    MoneySubsystem->AddSaleMoney(EMoneyRecordType::Coin, 50, CoinCount);

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

    int CoinCount = ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;
    
    TotalCoinCount -= ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum;
    ShopCoinSlotArray[SlotNum].CoinData.SameTypeCoinNum = 0;

    MoneySubsystem->AddSaleMoney(EMoneyRecordType::Coin, 50, CoinCount);
    
    OnCoinCountUpdate.Broadcast(SlotNum, 0);
    OnSetWeapon.Broadcast(-1);
}


void UShopCoinWSubsystem::ChangeCoinSlotByIndex(int32 SlotNum)
{
    if(SlotNum< ShopCoinSlotArray.Num())
    {
        CurrentCoinSlotIndex = SlotNum;
        OnCoinSlotChange.Broadcast();
    }
}


bool UShopCoinWSubsystem::GetCurrentCoinUnlock()
{
    return ShopCoinSlotArray[CurrentCoinSlotIndex].IsUnlock;
}

	
bool UShopCoinWSubsystem::UnlockCoinSlot(int32 SlotNum)
{
    if(10<SlotNum || SlotNum<0)
        return false;

    if(ShopCoinSlotArray[SlotNum].IsUnlock)
        return false;

    if(MoneySubsystem->SpendMoney(EMoneyRecordType::CoinSlot, 100))
    {
        ShopCoinSlotArray[SlotNum].IsUnlock = true;
        OnChangeCoinSlotCount.Broadcast(true);
        return true;
    }

    return false;
    
}
	
bool UShopCoinWSubsystem::IncreaseCoinSlot(int32 SlotNum)
{
    if(!ShopCoinSlotArray.IsValidIndex(SlotNum))
        return false;

    if(MoneySubsystem->SpendMoney(EMoneyRecordType::CoinSlot, 100))
    {
        
        ShopCoinSlotArray[SlotNum].IsUnlock = true;
        CurrentCoinSlotIndex = GetCurrentSlotCount()-1;
        OnChangeCoinSlotCount.Broadcast(true);
        OnCoinSlotChange.Broadcast();
        if(SlotNum<=0)
        {
            CurrentCoinSlotIndex = 0;
            OnCoinSlotChange.Broadcast();
        }
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

    if(CurrentCoinSlotIndex == SlotNum)
    {
        if(CurrentCoinSlotIndex-1 <0)
            CurrentCoinSlotIndex = 0;
        else
            CurrentCoinSlotIndex--;
        OnCoinSlotChange.Broadcast();
    }

    OnChangeCoinSlotCount.Broadcast(false);
    return true;
}

int32 UShopCoinWSubsystem::GetCurrentCoinCount()
{
    return ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.SameTypeCoinNum;
}


int32 UShopCoinWSubsystem::GetCurrentSlotNum()
{
    return CurrentCoinSlotIndex;
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
    
    if(!ShopCoinSlotArray[SlotNum].IsUnlock)
        return;

    CurrentCoinSlotIndex = SlotNum;
    ChangeCoinSide(true);
    OnCoinSlotChange.Broadcast();
    
}
	
void UShopCoinWSubsystem::ChangeCoinSide()
{
    if(IsCreateCoinFront)
        IsCreateCoinFront = false;
    else
        IsCreateCoinFront = true;

    OnChangeSlotCoinSide.Broadcast(IsCreateCoinFront);
}
	
void UShopCoinWSubsystem::ChangeCoinSide(bool ChangeFrontSide)
{
    IsCreateCoinFront = ChangeFrontSide;
    OnChangeSlotCoinSide.Broadcast(IsCreateCoinFront);
}

bool UShopCoinWSubsystem::GetIsCreateCoinFront()
{
    return IsCreateCoinFront;
}


void UShopCoinWSubsystem::SetWeaponToCoinSide(int32 WeaponID)
{
    if(CurrentCoinSlotIndex <0)
        return;
    
    if(IsTrySetSameWeapon(IsCreateCoinFront,WeaponID))
        return;

    /*
    if(IsCreateCoinFront)
    {
        if(ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.FrontWeaponID == -1)
        {
            if(GetSameWeaponInCoinSlot(CurrentCoinSlotIndex, WeaponID) != -1)
                return;
        }
    }
    else
    {
        if(ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.BackWeaponID == -1)
        {
            if(GetSameWeaponInCoinSlot(CurrentCoinSlotIndex, WeaponID) != -1)
                return;
        }
    }*/
    
    if(IsCreateCoinFront)
    {
        ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.FrontWeaponID = WeaponID;
    }
    else
    {
        ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.BackWeaponID = WeaponID;
    }
    OnSetWeapon.Broadcast(WeaponID);
}
	
int32 UShopCoinWSubsystem::GetCurrentCoinWeaponID(bool IsFront)
{
    if(IsFront)
    {
        if(ShopCoinSlotArray.IsValidIndex(CurrentCoinSlotIndex))
            return ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.FrontWeaponID;
        else
            return -1;
    }
    else
    {
        if(ShopCoinSlotArray.IsValidIndex(CurrentCoinSlotIndex))
            return ShopCoinSlotArray[CurrentCoinSlotIndex].CoinData.BackWeaponID;
        else
            return -1;
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
	
int32 UShopCoinWSubsystem::GetCurrentSlotCount()
{
    int32 Count = 0;

    for(int i =0; i< ShopCoinSlotArray.Num(); i++)
    {
        if(ShopCoinSlotArray[i].IsUnlock)
            Count++;
    }
    
    return Count;
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


int32 UShopCoinWSubsystem::GetUnlockSlotLastIndex()
{
    return LastUnlockCoinSlotIndex;
}