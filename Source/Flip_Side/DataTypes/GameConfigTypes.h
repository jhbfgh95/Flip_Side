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
