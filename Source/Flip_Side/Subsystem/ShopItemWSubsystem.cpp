// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopItemWSubsystem.h"

bool UShopItemWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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


void UShopItemWSubsystem::HoverShopItem(int32 ItemNum)
{
    OnItemHovered.Broadcast(ItemNum);
}
void UShopItemWSubsystem::UnHoverShopItem()
{
    OnItemUnHovered.Broadcast();
}
void UShopItemWSubsystem::HoverPlayerItem(int32 ItemNum)
{
    OnItemHovered.Broadcast(ItemNum);

}
void UShopItemWSubsystem::UnHoverPlayerItem(int32 ItemNum)
{

}
void UShopItemWSubsystem::BuyItem(int32 Num)
{

}
void UShopItemWSubsystem::SellItem(int32 Num)
{

}
