// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/WeaponLogicLibrary.h"

void UWeaponLogicLibrary::Test_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Do"));
}