// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Weapon_Action.h"

void UWeapon_Action::SetFinalAttackPoint(const float AttackPoint)
{
    if(AttackPoint < 0) FinalAttackPoint = 0;

    FinalAttackPoint = AttackPoint;
}

void UWeapon_Action::SetFinalBehaviorPoint(const float BehaviorPoint)
{
    if(BehaviorPoint < 0) FinalBehaviorPoint = 0;

    FinalBehaviorPoint = BehaviorPoint;
}

float UWeapon_Action::GetFinalAttackPoint() const
{
    return FinalAttackPoint;
}

float UWeapon_Action::GetFinalBehaviorPoint() const
{
    return FinalBehaviorPoint;
}

void UWeapon_Action::ExecuteAction()
{
    //UWeaponActionRegistryGISubsystem ActionRegistry = GetGameInstanceSubsystem()->GetSubsystem<UWeaponActionRegistryGISubsystem>();
    //if(!ActionRegistry) return;

    /*
    FWeaponLogic WeaponLogic = ActionRegistry->GetLogic(this->ActionID);

    if(WeaponLogic)
    {
        WeaponLogic(this);
    }
    */
}