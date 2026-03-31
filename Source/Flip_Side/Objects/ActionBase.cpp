// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"
#include "CoinActor.h"
#include "BossActor.h"
#include "DataTypes/GridTypes.h"

void UActionBase::CreateTestRange()
{
    FinalActionRange.GridX = 1;
    FinalActionRange.GridY = 1;
}

FGridPoint UActionBase::GetActionRange() const
{
    return FinalActionRange;
}

void UActionBase::SetActionRange(FGridPoint& ActionRange)
{
    FinalActionRange = ActionRange;
}

void UActionBase::SetInRangeCoins(ACoinActor* TargetCoin)
{
    if(TargetCoin)
    {
        InRangeCoins.Add(TargetCoin);
    }
    else
    {
        InRangeCoins.Empty();
    }
}

void UActionBase::SetInRangeBoss(ABossActor* TargetBoss)
{
    InRangeBoss = TargetBoss; // null이든 아니든 그냥 대입
}

TArray<ACoinActor*> UActionBase::GetInRangeCoins()
{
    return InRangeCoins;
}

bool UActionBase::GetInRangeBoss(ABossActor*& OutBoss) const
{
    if(InRangeBoss)
    {
        OutBoss = InRangeBoss;
        return true;
    }
    return false;
}

void UActionBase::SetLogicID(const int32 ID)
{
    LogicID = ID;
}

void UActionBase::ExecuteAction()
{
    UE_LOG(LogTemp, Warning, TEXT("Execute"));
}