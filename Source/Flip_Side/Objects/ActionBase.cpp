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
}

//이거로 편하게 배열검사하시라고..ㅎㅎ
bool UActionBase::CheckInCoinArray() const
{
    return InRangeCoins.IsEmpty();
}

void UActionBase::ExecuteAction()
{
    UE_LOG(LogTemp, Warning, TEXT("Execute"));
}