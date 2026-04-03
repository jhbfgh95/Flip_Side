// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_WarningSameWeapon.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"

void UW_WarningSameWeapon::NativeConstruct()
{
    Super::NativeConstruct();
    CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
    CoinCreateSubsystem->OnWarningSameWeapon.AddDynamic(this,&UW_WarningSameWeapon::OpenWarning);
}

void UW_WarningSameWeapon::NativeDestruct()
{
    CoinCreateSubsystem->OnWarningSameWeapon.RemoveAll(this);
    Super::NativeDestruct();
}
	

void UW_WarningSameWeapon::OpenWarning()
{
    PlayAnimation(WarningTextAnim);

}
