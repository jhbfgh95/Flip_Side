// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinViewModel.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"


void UShopCoinViewModel::Initialize()
{
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
}

void UShopCoinViewModel::BuyCoinSlot()
{
    CoinSubsystem->BuyCoinSlot();
}

void UShopCoinViewModel::SellCoinSlot()
{
    CoinSubsystem->SellCoinSlot();
}

void UShopCoinViewModel::IncreaseCoinSlotCoin(int32 SlotIndex,int32 Amount)
{
    CoinSubsystem->IncreaseCoinSlotCoin(SlotIndex, Amount);
}

void UShopCoinViewModel::DecreaseCoinSlotCoin(int32 SlotIndex,int32 Amount)
{
    CoinSubsystem->DecreaseCoinSlotCoin(SlotIndex, Amount);
}

void UShopCoinViewModel::SelectCoinSlot()
{

}

void UShopCoinViewModel::ChangeCoinSlotOrder()
{

}

int32 UShopCoinViewModel::GetUnlockCoinSlotCount()
{
    return CoinSubsystem->GetUnlockCoinSlotCount();
}

int32 UShopCoinViewModel::GetCurrentCoinSlotIndex()
{
    return CoinSubsystem->GetCurrentCoinSlotIndex();
}

int32 UShopCoinViewModel::GetCoinSlotCoinCount(int32 SlotNum)
{
    return 0;
}

FCoinTypeStructure UShopCoinViewModel::GetCoinSlotCoinType(int32 SlotNum)
{
    FCoinTypeStructure CoinTypeData;
    return CoinTypeData;
}


void UShopCoinViewModel::SetWeaponToCoinSide(int32 WeaponID, EWeaponClass WeaponClass)
{
    CoinSubsystem->SetWeaponToCoinSide(WeaponID, WeaponClass);
}
	
void UShopCoinViewModel::ChageCoinSide()
{
    CoinSubsystem->ChangeCoinSide();
}

bool UShopCoinViewModel::GetIsCoinFront()
{
    return CoinSubsystem->GetIsCreateCoinFront();
}


FFaceData UShopCoinViewModel::GetWeaponData(int32 WeaponID)
{
    FFaceData WeaponFaceData;
    DataManager->TryGetWeapon(WeaponID, WeaponFaceData);
    return WeaponFaceData;
}

const TArray<int32>& UShopCoinViewModel::GetUnlockWeaponArray(EWeaponClass WeaponClass)
{
    return UnlockSubsystem->GetUnlockWeaponArray(WeaponClass);
}