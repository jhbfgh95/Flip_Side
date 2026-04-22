// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinSlotContainer.h"
#include "Components/TextBlock.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

void UW_CoinSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this , &UW_CoinSlotContainer::SetTotalCoinText);
    SetTotalCoinText(0, 0);
}

void UW_CoinSlotContainer::NativeDestruct()
{
     ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_CoinSlotContainer::SetTotalCoinText(int32 ChangedSlotIndex, int32 Count)
{

    FString TextString = FString::Printf(TEXT("%d / 30"), ShopCoinSubsystem->GetTotalCoinCount());
    
    TotalCoinText->SetText(FText::FromString(TextString));
}