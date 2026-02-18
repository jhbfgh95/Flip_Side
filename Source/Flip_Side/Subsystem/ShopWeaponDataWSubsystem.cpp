// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopWeaponDataWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

bool UShopWeaponDataWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UShopWeaponDataWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UShopWeaponDataWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);

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


const FFaceData* UShopWeaponDataWSubsystem::GetWeaponDataByIndex(EWeaponClass WeponClass, int32 Index) const
{
    if(WeponClass == EWeaponClass::Tank )
    {
        if (TankWeapons)
        {
            if (!TankWeapons && !TankWeapons->IsValidIndex(Index))
                return nullptr;
            else 
                return &(*TankWeapons)[Index];
        }
    }
    else if(WeponClass == EWeaponClass::Deal )
    {
        if (DealWeapons)
        {
            if (!DealWeapons && !DealWeapons->IsValidIndex(Index))
                return nullptr;
            else 
                return &(*DealWeapons)[Index];
        }
    }
    else if(WeponClass == EWeaponClass::Heal )
    {
        if (UtilWeapons)
        {
            if (!UtilWeapons && !UtilWeapons->IsValidIndex(Index))
                return nullptr;
            else 
                return &(*UtilWeapons)[Index];
        }
    }

    return nullptr;
}

const FFaceData* UShopWeaponDataWSubsystem::GetWeaponDataByID(EWeaponClass weponClass, int32 ID) const
{
    if(weponClass == EWeaponClass::Tank )
    {
        if (TankWeapons)
        {
            for(int i =0; i< (*TankWeapons).Num(); i++)
            {
                if((*TankWeapons)[i].WeaponID == ID)
                {
                    return &(*TankWeapons)[i];
                }
            }
        }
    }
    else if(weponClass == EWeaponClass::Deal )
    {
        if (DealWeapons)
        {
            for(int i =0; i< (*DealWeapons).Num(); i++)
            {
                if((*DealWeapons)[i].WeaponID == ID)
                {
                    return &(*DealWeapons)[i];
                }
            }
        }

    }
    else if(weponClass == EWeaponClass::Heal )
    {

        if (UtilWeapons)
        {
            for(int i =0; i< (*UtilWeapons).Num(); i++)
            {
                if((*UtilWeapons)[i].WeaponID == ID)
                {
                    return &(*UtilWeapons)[i];
                }
            }
        }
    }

    return nullptr;
}

int32 UShopWeaponDataWSubsystem::GetWeaponIndexByID(EWeaponClass weponClass, int32 ID)
{
    if(weponClass == EWeaponClass::Tank )
    {
        if (TankWeapons)
        {
            for(int i =0; i< (*TankWeapons).Num(); i++)
            {
                if((*TankWeapons)[i].WeaponID == ID)
                {
                    return i;
                }
            }
        }
    }
    else if(weponClass == EWeaponClass::Deal )
    {
        if (DealWeapons)
        {
            for(int i =0; i< (*DealWeapons).Num(); i++)
            {
                if((*DealWeapons)[i].WeaponID == ID)
                {
                    return i;
                }
            }
        }

    }
    else if(weponClass == EWeaponClass::Heal )
    {

        if (UtilWeapons)
        {
            for(int i =0; i< (*UtilWeapons).Num(); i++)
            {
                if((*UtilWeapons)[i].WeaponID == ID)
                {
                    return i;
                }
            }
        }
    }

    return -1;
}

int32 UShopWeaponDataWSubsystem::GetWeaponArrayNum(EWeaponClass weponClass)
{
    if(weponClass == EWeaponClass::Tank )
    {
        if (TankWeapons)
        {
            return TankWeapons->Num();
        }
    }
    else if(weponClass == EWeaponClass::Deal )
    {
        if (DealWeapons)
        {
            return DealWeapons->Num();
        }

    }
    else if(weponClass == EWeaponClass::Heal )
    {

        if (UtilWeapons)
        {
            return UtilWeapons->Num();
        }
    }

    return -1;
}