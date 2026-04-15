// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinCountButton.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "Components/Button.h"

void UW_CoinCountButton::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

    CountButton->OnClicked.AddDynamic(this, &UW_CoinCountButton::ChangeCount);
}


void UW_CoinCountButton::InitButton(int32 SlotNum)
{
    SlotIndex = SlotNum;
}

void UW_CoinCountButton::ChangeCount()
{
    if(IsIncrease)
    {
        ShopCoinSubsystem->IncreaseSlotCoinCount(SlotIndex);
    }
    else
    {
        ShopCoinSubsystem->DecreaseSlotCoinCount(SlotIndex);
    }
}