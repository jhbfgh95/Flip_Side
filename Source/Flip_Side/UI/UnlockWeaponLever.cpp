// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UnlockWeaponLever.h"
#include "Subsystem/ShopUnlockWeaponWSubsystem.h"
void AUnlockWeaponLever::BeginPlay()
{
    Super::BeginPlay();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();

}

void AUnlockWeaponLever::ExecuteLeverLogic()
{
    UnlockWeaponSubsystem->UnlockCurrentWeapon();
}