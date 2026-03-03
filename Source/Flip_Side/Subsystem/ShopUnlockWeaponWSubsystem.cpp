// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

void UShopUnlockWeaponWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
}
//무기 해금

void UShopUnlockWeaponWSubsystem::UnlockWeaponByIndex(int32 WeaponIndex)
{


}

//무기 선택
void UShopUnlockWeaponWSubsystem::SelectUnlockWeaponByIndex(EWeaponClass WeaponClass, int32 WeaponIndex, bool IsWeaponUnlock)
{
    if(WeaponIndex == CurrentUnlockWeaponIndex)
        return;
    CurrentUnlockWeaponIndex = WeaponIndex;
    CurrentUnlockWeaponClass = WeaponClass;
    OnSelectUnlockWeapon.Broadcast(WeaponClass, WeaponIndex, IsWeaponUnlock);

}

int32 UShopUnlockWeaponWSubsystem::GetCurrentUnlockWeaponIndex()
{
    return CurrentUnlockWeaponIndex;
}


void UShopUnlockWeaponWSubsystem::UnlockCurrentWeapon()
{
    if(CurrentUnlockWeaponIndex != -1)
    {
        UnlockSubsystem->UnlockWeapon(CurrentUnlockWeaponClass, CurrentUnlockWeaponIndex);
    }
    
}

EWeaponClass UShopUnlockWeaponWSubsystem::GetCurrentWeaponClass()
{
    return CurrentUnlockWeaponClass;
}