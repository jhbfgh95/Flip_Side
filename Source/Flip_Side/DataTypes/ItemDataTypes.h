// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemRange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemEffectValue = 0;
};

/**
 * 아이템 선택 결과 정보
 */
USTRUCT(BlueprintType)
struct FSelectItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SameItemNum = 0;
};