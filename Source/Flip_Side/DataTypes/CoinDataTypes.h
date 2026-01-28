// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.generated.h"

/**
 * 같은 타입의 코인 묶음 정보
 * (Front / Back 무기 ID 쌍)
 */
USTRUCT(BlueprintType)
struct FCoinTypeStructure
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FrontWeaponID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BackWeaponID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SameTypeCoinNum = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SlotNum = 0;
};


/**
 * 전투 중 코인 1개의 상태 정보
 */
USTRUCT(BlueprintType)
struct FBattleCoinInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGridPoint CoinGrid;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SelectedWeaponID = 0;
};


USTRUCT(BlueprintType)
struct FRandomState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EFaceState RandomFace;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGridPoint RandomGrid;
};