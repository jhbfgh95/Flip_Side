// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponWarning.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"

void UW_UnlockWeaponWarning::NativeConstruct()
{
    Super::NativeConstruct();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    UnlockWeaponSubsystem->OnUnlockWeaponWarning.AddDynamic(this,&UW_UnlockWeaponWarning::SetWarning);
    
}

void UW_UnlockWeaponWarning::NativeDestruct()
{
    UnlockWeaponSubsystem->OnUnlockWeaponWarning.RemoveAll(this);
    Super::NativeDestruct();
}
	

void UW_UnlockWeaponWarning::SetWarning(int32 WarningCode)
{
    FString text;

    switch (WarningCode)
    {
     case 0:
        text = TEXT("돈이 부족합니다.");
        break;
    
    default:
        break;
    }
    ActiveWarning(text);
}



