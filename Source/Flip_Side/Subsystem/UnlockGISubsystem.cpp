// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/UnlockGISubsystem.h"

void UUnlockGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
}

int32 UUnlockGISubsystem::GetUnlockCardID(int32 index)
{
    if(CardUnlockArray.Num() <= index)
        return -1;
    return CardUnlockArray[index];
}
	
int32 UUnlockGISubsystem::GetUnlockWeaponID(EWeaponClass WeaponClass, int32 index)
{
    if(EWeaponClass::Tank == WeaponClass &&TankUnlockArray.Num()<= index)
    {
        return TankUnlockArray[index];
    }
    else if(EWeaponClass::Deal == WeaponClass&&TankUnlockArray.Num()<= index)
    {
        return DealUnlockArray[index];
    }
    else if(EWeaponClass::Heal == WeaponClass&&TankUnlockArray.Num()<= index)
    {
        return UtilUnlockArray[index];
    }
    return -1;
}

void UUnlockGISubsystem::UnlockWeapon(EWeaponClass WeaponClass, int32 Index)
{
    if(EWeaponClass::Tank == WeaponClass)
    {
        if(GetWeaponUnlockIndexByIndex(WeaponClass, Index) == -1)
        {
            TankUnlockArray.Add(Index);
            OnWeaponUnlock.Broadcast(WeaponClass, Index);
        }
        
    }
    else if(EWeaponClass::Deal == WeaponClass)
    {
        if(GetWeaponUnlockIndexByIndex(WeaponClass, Index) == -1)
        {
            DealUnlockArray.Add(Index);
            OnWeaponUnlock.Broadcast(WeaponClass, Index);
        }
    }
    else if(EWeaponClass::Heal == WeaponClass)
    {
        if(GetWeaponUnlockIndexByIndex(WeaponClass, Index)==-1)
        {
            UtilUnlockArray.Add(Index);
            OnWeaponUnlock.Broadcast(WeaponClass, Index);
        }
    }
}

void UUnlockGISubsystem::UnlockCard(int32 ID)
{
    CardUnlockArray.Add(ID);
}


int32 UUnlockGISubsystem::GetUnlockWeaponArrrayNum(EWeaponClass WeaponClass)
{
    switch(WeaponClass)
    {
        case EWeaponClass::Tank:
            return TankUnlockArray.Num();
            break;
        case EWeaponClass::Deal:
            return DealUnlockArray.Num();
            break;
        case EWeaponClass::Heal:
            return UtilUnlockArray.Num();
            break;
        default:
            return -1;
    }

}

int32 UUnlockGISubsystem::GetUnlockCardArrayNum()
{
     return CardUnlockArray.Num();
}


int32 UUnlockGISubsystem::GetWeaponUnlockIndexByIndex(EWeaponClass WeaponClass, int32 index)
{

    if(EWeaponClass::Tank == WeaponClass)
    {
       return TankUnlockArray.Find(index);
    }
    else if(EWeaponClass::Deal == WeaponClass)
    {
        return DealUnlockArray.Find(index);
    }
    else if(EWeaponClass::Heal == WeaponClass)
    {
       return UtilUnlockArray.Find(index);
    }
    else
    {
        return -1;
    }
}


int32 UUnlockGISubsystem::GetUnlockWeaponIndex(EWeaponClass WeaponClass, int32 index)
{
    if(EWeaponClass::Tank == WeaponClass &&TankUnlockArray.IsValidIndex(index))
    {
        return TankUnlockArray[index];
    }
    else if(EWeaponClass::Deal == WeaponClass&&DealUnlockArray.IsValidIndex(index))
    {
        return DealUnlockArray[index];
    }
    else if(EWeaponClass::Heal == WeaponClass&&UtilUnlockArray.IsValidIndex(index))
    {
        return UtilUnlockArray[index];
    }
    return -1;
}

bool UUnlockGISubsystem::IsCardUnlockByID(int32 ID)
{
    if(CardUnlockArray.Find(ID) ==-1)
            return false;
        else
            return true;
}