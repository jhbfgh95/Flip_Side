// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FBossData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossHP = 0;
};
