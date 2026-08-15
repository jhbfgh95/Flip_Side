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
        
        UnlockWeapons.Append(*TankWeapons);
        UnlockWeapons.Append(*DealWeapons);
        UnlockWeapons.Append(*UtilWeapons);

        for(int i = UnlockWeapons.Num()-1; 0 <= i ; i--)
        {
            if(UnlockWeapons[i].WeaponID == -2 || UnlockWeapons[i].WeaponID == -3 )
            {
                UnlockWeapons.RemoveAt(i);
            }
        }
    }
}

void UShopUnlockWeaponWSubsystem::SelectUnlockWeapon(int32 WeaponID)
{
    CurrentUnlockWeaponID = WeaponID;
    OnSelectUnlockWeapon.Broadcast(CurrentUnlockWeaponID, true);
}

void UShopUnlockWeaponWSubsystem::UnSelectWeapon()
{
    OnUnselectUnlockWeapon.Broadcast();
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
            UnlockSubsystem->UnlockWeapon(CurrentUnlockWeaponID);
        }
    }
}

EWeaponClass UShopUnlockWeaponWSubsystem::GetCurrentWeaponClass()
{
    return CurrentUnlockWeaponClass;
}

void UShopUnlockWeaponWSubsystem::UnlockWeaponWarning(int32 WarningCode)
{
    OnUnlockWeaponWarning.Broadcast(WarningCode);
}
	
const TArray<FFaceData> UShopUnlockWeaponWSubsystem::GetUnlockWeapons()
{
    return UnlockWeapons;
}