// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardTypes.generated.h"

USTRUCT(BlueprintType)
struct FCoinCardModifiers
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) int32 ExtraActions = 0;
    UPROPERTY(BlueprintReadOnly) int32 AttackAdd = 0;
    UPROPERTY(BlueprintReadOnly) int32 RangeAdd = 0;
    UPROPERTY(BlueprintReadOnly) int32 BehaviorAdd = 0;
    UPROPERTY(BlueprintReadOnly) bool bLifeSteal = false;
};

USTRUCT(BlueprintType)
struct FCardData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CardID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString CardName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Card_Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon = nullptr;

    // DB에서 로드되는 카드 수치 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TriggerCount = 0;   // 발동에 필요한 코인 수 (0 = 수량 조건 없음)

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackAdd = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BehaviorAdd = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RangeAdd = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ExtraActions = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLifeSteal = false;
};