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
    UPROPERTY(BlueprintReadOnly) int32 CountAdd = 0;
    UPROPERTY(BlueprintReadOnly) int32 AbilityRangeAdd = 0;
    UPROPERTY(BlueprintReadOnly) bool bLifeSteal = false;

    bool operator==(const FCoinCardModifiers& Other) const
    {
        return ExtraActions == Other.ExtraActions && AttackAdd == Other.AttackAdd &&
            BehaviorAdd == Other.BehaviorAdd && CountAdd == Other.CountAdd &&
            RangeAdd == Other.RangeAdd && AbilityRangeAdd == Other.AbilityRangeAdd &&
            bLifeSteal == Other.bLifeSteal;
    }
};

USTRUCT(BlueprintType)
struct FCardGoldTierData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) int32 MinimumGold = 0;
    UPROPERTY(BlueprintReadOnly) FCoinCardModifiers Modifiers;
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

    // RangeAdd는 공격 사거리입니다. 능력 사거리와 횟수는 독립적으로 적용합니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AbilityRangeAdd = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CountAdd = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TriggerRange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RequiredWeaponID = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    TArray<FCardGoldTierData> GoldTiers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ExtraActions = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLifeSteal = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Price = 0;
};

// Battle HUD 표시 전용 카드 슬롯 데이터입니다.
USTRUCT(BlueprintType)
struct FBattleCardSlotViewData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 SlotNumber = INDEX_NONE;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FCardData CardData;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bOccupied = false;

    // 위젯 구현과 별개로 카드 관리자가 실제 조건 충족 상태를 제공합니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsActive = false;
};
