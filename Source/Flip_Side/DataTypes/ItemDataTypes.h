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
    int32 ItemTypeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemRange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemEffectValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Item_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* ItemIcon=nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor;

    // DB에서 읽은 코드 문자열
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BehaviorCode;
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
