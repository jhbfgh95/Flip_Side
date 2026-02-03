// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

bool UCoinCreateWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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


void UCoinCreateWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
    }

    if(DM)
    {
        if (DM->TryGetWeaponsByType(1, TankWeapons))
        {
            for (const FFaceData& Weapon : *TankWeapons)
            {
                UE_LOG(LogTemp, Log, TEXT("WeaponID=%d"), Weapon.WeaponID);
            }
        }
        if (DM->TryGetWeaponsByType(2, DealWeapons))
        {
            for (const FFaceData& Weapon : *DealWeapons)
            {
                UE_LOG(LogTemp, Log, TEXT("WeaponID=%d"), Weapon.WeaponID);
            }
        } 
        if (DM->TryGetWeaponsByType(3, UtilWeapons))
        {
            for (const FFaceData& Weapon : *UtilWeapons)
            {
                UE_LOG(LogTemp, Log, TEXT("WeaponID=%d"), Weapon.WeaponID);
            }
        } 
    }
}



//코인의 앞뒤를 변경
void UCoinCreateWSubsystem::ChangeCoinSide()
{
    if(bIsCreateCoinFront)
    {
        bIsCreateCoinFront = false;
        //OnSelectedCoinUpdate.Broadcast(SelectedCoin.FrontWeaponID);
    }
    else
    {
        bIsCreateCoinFront = true;
        //OnSelectedCoinUpdate.Broadcast(SelectedCoin.BackWeaponID);
    }
        

}

FCoinTypeStructure UCoinCreateWSubsystem::GetSelectCoin()
{
    return SelectedCoin;
}

//코인 선택
void UCoinCreateWSubsystem::SelectCoin(FCoinTypeStructure SelectCoinInfo, EWeaponClass CoinClass)
{
    SelectedCoin = SelectCoinInfo;
    SetCoinClass(CoinClass);
    bIsCreateCoinFront = true;

    OnSelectedCoin.Broadcast(SelectedCoin, CoinClass);
    OnCoinClassUpdate.Broadcast(CoinClass);
}

//선택된 코인 면의 ID를 변경
void UCoinCreateWSubsystem::ChangeSelectedCoinWeapon(int32 WeaponID)
{
    if(bIsCreateCoinFront)
    {
        SelectedCoin.FrontWeaponID = WeaponID;
    }
    else
    {
        SelectedCoin.BackWeaponID = WeaponID;
    }
    OnSelectedCoinUpdate.Broadcast(WeaponID);
}


void UCoinCreateWSubsystem::SetCoinClass(EWeaponClass weponClass)
{
    if(SelectedCoinClass != weponClass)
    {
        SelectedCoinClass = weponClass;
        
        //SelectedCoin.FrontWeaponID = -1;
        //SelectedCoin.BackWeaponID = -1;
    }
    OnCoinClassUpdate.Broadcast(weponClass);
}


bool UCoinCreateWSubsystem::GetIsCreateCoinFront()
{
    return bIsCreateCoinFront;
}


EWeaponClass UCoinCreateWSubsystem::GetSelectCoinClass()
{
    return SelectedCoinClass;
}


void UCoinCreateWSubsystem::OnClassSelectMode()
{
    OnCoinClassSelectMode.Broadcast();
}


void UCoinCreateWSubsystem::OffClassSelectMode()
{
    OffCoinClassSelectMode.Broadcast();
}


const FFaceData* UCoinCreateWSubsystem::GetTankWeaponData(int32 ID) const
{
    if (!TankWeapons || !TankWeapons->IsValidIndex(ID))
        return nullptr;
    else 
        return &(*TankWeapons)[ID];
    
    
}

const FFaceData* UCoinCreateWSubsystem::GetDealWeaponData(int32 ID) const
{
    if (!DealWeapons || !DealWeapons->IsValidIndex(ID))
        return nullptr;
    else 
        return &(*DealWeapons)[ID];
}

const FFaceData* UCoinCreateWSubsystem::GetUtilWeaponData(int32 ID) const
{
    if (!UtilWeapons || !UtilWeapons->IsValidIndex(ID))
        return nullptr;
    else 
        return &(*UtilWeapons)[ID];
}