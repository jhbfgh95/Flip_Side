// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"
#include "CoinActor.h"
#include "BossActor.h"
#include "Base_OtherActor.h"
#include "DataTypes/GridTypes.h"

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

void UActionBase::SetInRangeOthers(ABase_OtherActor* TargetOthers)
{
    if(TargetOthers)
    {
        InRangeOthers.Add(TargetOthers);
    }
    else
    {
        InRangeOthers.Empty();
    }
}

TArray<ACoinActor*> UActionBase::GetInRangeCoins()
{
    return InRangeCoins;
}

void UActionBase::SetLogicID(const int32 ID)
{
    LogicID = ID;
}

void UActionBase::ExecuteAction()
{
}
