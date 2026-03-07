// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/UnlockWeaponLever.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
void AUnlockWeaponLever::BeginPlay()
{
    Super::BeginPlay();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();

}

void AUnlockWeaponLever::ExecuteLeverLogic()
{
    UnlockWeaponSubsystem->UnlockCurrentWeapon();
}