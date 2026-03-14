// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Weapon_Action.h"
#include "ActionLogicRegistryGISubsystem.h"
#include "Engine/World.h"
#include "DataManagerSubsystem.h"
#include "WeaponLogicLibrary.h"

void UWeapon_Action::SetFinalAttackPoint(const int32 AttackPoint)
{
    if(AttackPoint < 0) FinalAttackPoint = 0;

    FinalAttackPoint = AttackPoint;
}

void UWeapon_Action::SetFinalBehaviorPoint(const int32 BehaviorPoint)
{
    if(BehaviorPoint < 0) { FinalBehaviorPoint = 0;}
    else{ FinalBehaviorPoint = BehaviorPoint; }

}

int32 UWeapon_Action::GetFinalAttackPoint() const
{
    return FinalAttackPoint;
}

int32 UWeapon_Action::GetFinalBehaviorPoint() const
{
    return FinalBehaviorPoint;
}

void UWeapon_Action::ExecuteAction()
{
    if(LogicID == -1) return;

    UWorld* World = GetWorld();
    if (!World) return;

    UActionLogicRegistryGISubsystem* ActionRegistry = World->GetGameInstance()->GetSubsystem<UActionLogicRegistryGISubsystem>();
    if(!ActionRegistry) return;

    SetWeaponData();
 
    FActionLogic WeaponLogic = ActionRegistry->GetWeaponLogic(this->LogicID);

    if(WeaponLogic)
    {
        WeaponLogic(this);
        UE_LOG(LogTemp, Warning, TEXT("7"));
    }
}

void UWeapon_Action::SetSingleCellTargetCoin(ACoinActor* TargetCoin)
{
    InRangeCoins.Empty();
    InRangeCoins.Add(TargetCoin);

    SetWeaponData();
}

void UWeapon_Action::SetWeaponData()
{
    auto* DM = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (DM && DM->IsCacheReady())
    {
        DM->TryGetWeapon(LogicID, WeaponData);
    }
}