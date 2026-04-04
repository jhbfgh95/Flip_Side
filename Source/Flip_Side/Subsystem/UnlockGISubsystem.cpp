// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/UnlockGISubsystem.h"

void UUnlockGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if(DealUnlockArray.Num()<=0)
    {
        DealUnlockArray.Add(1);
    }
}

int32 UUnlockGISubsystem::GetUnlockCardID(int32 index)
{
    if(CardUnlockArray.Num() <= index)
        return -1;
    return CardUnlockArray[index];
}
	
int32 UUnlockGISubsystem::GetUnlockWeaponID(EWeaponClass WeaponClass, int32 index)
{
    if(EWeaponClass::Tank == WeaponClass && index< TankUnlockArray.Num())
    {
        return TankUnlockArray[index];
    }
    else if(EWeaponClass::Deal == WeaponClass&& index<DealUnlockArray.Num())
    {
        return DealUnlockArray[index];
    }
    else if(EWeaponClass::Heal == WeaponClass&& index<UtilUnlockArray.Num())
    {
        return UtilUnlockArray[index];
    }
    return -1;
}

void UUnlockGISubsystem::UnlockWeapon(EWeaponClass WeaponClass, int32 ID)
{
    if(EWeaponClass::Tank == WeaponClass)
    {
        if(GetWeaponUnlockIndexByIndex(WeaponClass, ID) == -1)
        {
            TankUnlockArray.Add(ID);
            OnWeaponUnlock.Broadcast(WeaponClass, ID);
        }
        
    }
    else if(EWeaponClass::Deal == WeaponClass)
    {
        if(GetWeaponUnlockIndexByIndex(WeaponClass, ID) == -1)
        {
            DealUnlockArray.Add(ID);
            OnWeaponUnlock.Broadcast(WeaponClass, ID);
        }
    }
    else if(EWeaponClass::Heal == WeaponClass)
    {
        if(GetWeaponUnlockIndexByIndex(WeaponClass, ID)==-1)
        {
            UtilUnlockArray.Add(ID);
            OnWeaponUnlock.Broadcast(WeaponClass, ID);
        }
    }
}

void UUnlockGISubsystem::UnlockCard(int32 ID)
{
    if(CardUnlockArray.Find(ID) == -1)
    {
        CardUnlockArray.Add(ID);
        OnUnlockCard.Broadcast(ID);
    }
        
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

bool UUnlockGISubsystem::IsWeaponUnlockByID(EWeaponClass WeaponClass, int32 ID)
{
    if(EWeaponClass::Tank == WeaponClass)
    {
       if(TankUnlockArray.Find(ID)==-1)
            return false;
        else
            return true;
    }
    else if(EWeaponClass::Deal == WeaponClass)
    {
        if(DealUnlockArray.Find(ID)==-1)
            return false;
        else
            return true;
    }
    else if(EWeaponClass::Heal == WeaponClass)
    {
        if(UtilUnlockArray.Find(ID)==-1)
            return false;
        else
            return true;
    }
    else
    {
        return false;
    }
}