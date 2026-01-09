// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.generated.h"


USTRUCT(BlueprintType)
struct FGridPoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GridX = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GridY = 0;
};