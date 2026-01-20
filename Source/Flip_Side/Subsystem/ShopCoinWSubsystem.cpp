// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopCoinWSubsystem.h"

bool UShopCoinWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel"));
} 



bool UShopCoinWSubsystem::CanAddCoin()
{
    if(TotalCoinCount <= MAX_TOTAL_COIN)
    {
        return true;
    }
    return false;
}



void UShopCoinWSubsystem::AddTotalCoin()
{
    if(TotalCoinCount <= MAX_TOTAL_COIN)
    {
        TotalCoinCount++;
    }
}
void UShopCoinWSubsystem::RemoveTotalCoin()
{
    if(0<TotalCoinCount)
    {
        TotalCoinCount--;
    }
}