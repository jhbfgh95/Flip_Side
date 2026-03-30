// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinSlotNum.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

void UW_CoinSlotNum::NativeConstruct()
{
    Super::NativeConstruct();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

    CoinSubsystem->OnCoinSlotChange.AddDynamic(this,&UW_CoinSlotNum::SetSlotNumText);

    
    SetSlotNumText(true);

}

void UW_CoinSlotNum::NativeDestruct()
{
    CoinSubsystem->OnChangeSlotCoinSide.RemoveAll(this);
    Super::NativeDestruct();
}
	
void UW_CoinSlotNum::SetSlotNumText(bool IsChangeToBottom)
{
    SetCountText(CoinSubsystem->GetCurrentSlotNum()+1);
}