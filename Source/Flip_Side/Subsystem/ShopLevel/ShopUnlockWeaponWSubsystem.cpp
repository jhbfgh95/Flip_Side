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
    UGameInstance* GI = World.GetGameInstance();
    if (!IsValid(GI)) return;

    MoneySubsystem = GI->GetSubsystem<UMoneyGISubsystem>();
    UnlockSubsystem = GI->GetSubsystem<UUnlockGISubsystem>();
    DM = GI->GetSubsystem<UDataManagerSubsystem>();
    UnlockWeapons.Reset();
    if(IsValid(DM))
    {
        if (!DM->TryGetWeaponsByType(1, TankWeapons) ||
            !DM->TryGetWeaponsByType(2, DealWeapons) ||
            !DM->TryGetWeaponsByType(3, UtilWeapons))
        {
            UE_LOG(LogTemp, Error, TEXT("[ShopUnlockWeapon] Weapon data is unavailable. Check Content/DB.db and the weapon cache."));
            return;
        }
        
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

bool UShopUnlockWeaponWSubsystem::UnlockWeapon(int32 WeaponID)
{
    if(WeaponID != -1 && IsValid(DM) && IsValid(MoneySubsystem) && IsValid(UnlockSubsystem))
    {
        FFaceData WeaponFaceData;
        if(!DM->TryGetWeapon(WeaponID,WeaponFaceData))
            return false;

        if(MoneySubsystem->SpendMoney(EMoneyRecordType::Weapon, WeaponFaceData.Price))
        {
            UnlockSubsystem->UnlockWeapon(WeaponID);
            return true;
        }
    }
    return false;
}

void UShopUnlockWeaponWSubsystem::UnlockWeaponWarning(int32 WarningCode)
{
    OnUnlockWeaponWarning.Broadcast(WarningCode);
}
	
const TArray<FFaceData> UShopUnlockWeaponWSubsystem::GetUnlockWeapons()
{
    return UnlockWeapons;
}
