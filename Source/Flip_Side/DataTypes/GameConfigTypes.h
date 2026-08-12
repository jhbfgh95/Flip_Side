// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameConfigTypes.generated.h"

USTRUCT(BlueprintType)
struct FStageRewardData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 StageID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RewardGold = 0;
};

// 코인 슬롯 레벨(1~3)별 구매 비용/HP. 슬롯은 이 3개 레벨 중 하나를 구매.
USTRUCT(BlueprintType)
struct FCoinSlotLevelTier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Cost = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;
};

USTRUCT(BlueprintType)
struct FGameConfigData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 InitialGold = 800;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinSlotUnlockCost = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinCountAddCost = 50;
};
