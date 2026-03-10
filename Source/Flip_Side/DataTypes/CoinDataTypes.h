// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.generated.h"

/**
 * ���� Ÿ���� ���� ���� ����
 * (Front / Back ���� ID ��)
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
 * ���� �� ���� 1���� ���� ����
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

USTRUCT(BlueprintType)
struct FCCStructure
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECCTypes CCType = ECCTypes::None;

    //지속시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CCDuration = 0;

    //로직은 순수 함수로 추가할 것.
};

