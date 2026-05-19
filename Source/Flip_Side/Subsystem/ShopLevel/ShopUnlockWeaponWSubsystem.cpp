// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"

bool UShopUnlockWeaponWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UShopUnlockWeaponWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);
    MoneySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMoneyGISubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    DM = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if(DM)
    {
        DM->TryGetWeaponsByType(1, TankWeapons);
        DM->TryGetWeaponsByType(2, DealWeapons);
        DM->TryGetWeaponsByType(3, UtilWeapons);
    }
}
//무기 선택
void UShopUnlockWeaponWSubsystem::SelectUnlockWeapon(EWeaponClass WeaponClass, int32 WeaponID, bool IsWeaponUnlock)
{
    if(WeaponID == CurrentUnlockWeaponID)
        return;

    CurrentUnlockWeaponID = WeaponID;
    CurrentUnlockWeaponClass = WeaponClass;

    OnSelectUnlockWeapon.Broadcast(WeaponClass, WeaponID, IsWeaponUnlock);

}

int32 UShopUnlockWeaponWSubsystem::GetCurrentUnlockWeaponID()
{
    return CurrentUnlockWeaponID;
}


void UShopUnlockWeaponWSubsystem::UnlockCurrentWeapon()
{
    if(CurrentUnlockWeaponID != -1)
    {
        FFaceData WeaponFaceData;
        if(!DM->TryGetWeapon(CurrentUnlockWeaponID,WeaponFaceData))
            return;

        if(MoneySubsystem->SpendMoney(EMoneyRecordType::Weapon, WeaponFaceData.Price))
        {
            UnlockSubsystem->UnlockWeapon(CurrentUnlockWeaponClass, CurrentUnlockWeaponID);
            OnSelectUnlockWeapon.Broadcast(CurrentUnlockWeaponClass,CurrentUnlockWeaponID,true);
        }
       
    }
    
}

EWeaponClass UShopUnlockWeaponWSubsystem::GetCurrentWeaponClass()
{
    return CurrentUnlockWeaponClass;
}

void UShopUnlockWeaponWSubsystem::ChangeUnlockWeaponClass(EWeaponClass WeaponClass)
{
    OnChangeUnlockWaeponClass.Broadcast(WeaponClass);
}
	
int32 UShopUnlockWeaponWSubsystem::GetWeaponArrayNum(EWeaponClass WeaponType)
{
    if(WeaponType == EWeaponClass::Tank )
    {
        if (TankWeapons)
        {
            return TankWeapons->Num();
        }
    }
    else if(WeaponType == EWeaponClass::Deal )
    {
        if (DealWeapons)
        {
            return DealWeapons->Num();
        }

    }
    else if(WeaponType == EWeaponClass::Heal )
    {

        if (UtilWeapons)
        {
            return UtilWeapons->Num();
        }
    }

    return -1;
}

int32 UShopUnlockWeaponWSubsystem::GetWeaponIDByIndex(EWeaponClass WeaponType,int32 Index)
{
    if(Index <=-1)
        return -1;
    if(WeaponType == EWeaponClass::Tank )
    {
        if (TankWeapons && Index < TankWeapons->Num())
        {
            return (*TankWeapons)[Index].WeaponID;
        }
    }
    else if(WeaponType == EWeaponClass::Deal )
    {
        if (DealWeapons&& Index < DealWeapons->Num())
        {
            return (*DealWeapons)[Index].WeaponID;
        }

    }
    else if(WeaponType == EWeaponClass::Heal )
    {

        if (UtilWeapons&&Index < UtilWeapons->Num())
        {
            return (*UtilWeapons)[Index].WeaponID;
        }
    }

    return -1;
}
	
void UShopUnlockWeaponWSubsystem::UnlockWeaponWarning(int32 WarningCode)
{
    OnUnlockWeaponWarning.Broadcast(WarningCode);
}