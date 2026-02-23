// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopWeaponDataWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
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
                TankWeaponUnlockArray.Add(false);
            }
        }
        if (DM->TryGetWeaponsByType(2, DealWeapons))
        {
   
            for (const FFaceData& Weapon : *DealWeapons)
            {
                DealWeaponUnlockArray.Add(false);
            }
        } 
        if (DM->TryGetWeaponsByType(3, UtilWeapons))
        {
            for (const FFaceData& Weapon : *UtilWeapons)
            {
                UtilWeaponUnlockArray.Add(false);
            }
        } 
    }

    if (UGameInstance* WGI = GetWorld()->GetGameInstance())
    {
        UnlockSubsystem = WGI->GetSubsystem<UUnlockGISubsystem>();
    }

    if(UnlockSubsystem)
    {
        for(int i =0; i< TankWeaponUnlockArray.Num();i++)
        {
            if(UnlockSubsystem->GetWeaponUnlockIndexByIndex(EWeaponClass::Tank, i)!= -1)
            {
                TankWeaponUnlockArray[i] = true;
            }
        }
        for(int i =0; i< DealWeaponUnlockArray.Num();i++)
        {
            if(UnlockSubsystem->GetWeaponUnlockIndexByIndex(EWeaponClass::Deal, i)!= -1)
            {
                DealWeaponUnlockArray[i] = true;
            }
        }
        for(int i =0; i< UtilWeaponUnlockArray.Num();i++)
        {
            if(UnlockSubsystem->GetWeaponUnlockIndexByIndex(EWeaponClass::Heal, i)!= -1)
            {
                UtilWeaponUnlockArray[i] = true;
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
            if (TankWeapons->IsValidIndex(Index))
                return &(*TankWeapons)[Index];
            else 
                return nullptr;
        }
    }
    else if(WeponClass == EWeaponClass::Deal )
    {
        if (DealWeapons)
        {
            if (!DealWeapons->IsValidIndex(Index))
                return &(*DealWeapons)[Index];
            else 
                return nullptr;
        }
    }
    else if(WeponClass == EWeaponClass::Heal )
    {
        if (UtilWeapons)
        {
            if (UtilWeapons->IsValidIndex(Index))
                return &(*UtilWeapons)[Index];
            else 
                return nullptr;
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


bool UShopWeaponDataWSubsystem::IsWeaponUnlockByIndex(EWeaponClass WeaponClass, int32 Index)
{
    if(WeaponClass == EWeaponClass::Tank )
    {
        if (Index<TankWeaponUnlockArray.Num())
        {
            return TankWeaponUnlockArray[Index];
        }
    }
    else if(WeaponClass == EWeaponClass::Deal )
    {
       if (Index<DealWeaponUnlockArray.Num())
        {
            return DealWeaponUnlockArray[Index];
        }

    }
    else if(WeaponClass == EWeaponClass::Heal )
    {
        if (Index<UtilWeaponUnlockArray.Num())
        {
            return UtilWeaponUnlockArray[Index];
        }
    }

    return false;
}


void UShopWeaponDataWSubsystem::UnlockWeaponByIndex(EWeaponClass WeaponClass, int32 Index)
{
    if(WeaponClass == EWeaponClass::Tank )
    {
        if (Index<TankWeaponUnlockArray.Num())
        {
            TankWeaponUnlockArray[Index] =true;
        }
    }
    else if(WeaponClass == EWeaponClass::Deal )
    {
       if (Index<DealWeaponUnlockArray.Num())
        {
            DealWeaponUnlockArray[Index]=true;
        }

    }
    else if(WeaponClass == EWeaponClass::Heal )
    {
        if (Index<UtilWeaponUnlockArray.Num())
        {
            UtilWeaponUnlockArray[Index]=true;
        }
    }
}