// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"
#include "CoinActor.h"
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
    UE_LOG(LogTemp, Warning, TEXT("Execute"));
}