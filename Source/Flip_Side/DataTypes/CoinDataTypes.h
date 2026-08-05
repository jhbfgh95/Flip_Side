// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.generated.h"

class UTexture2D;

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

/** CoinReadyPhase에서 UI 슬롯이 관리하는 코인 묶음 데이터입니다. */
USTRUCT(BlueprintType)
struct FBattleCoinSlotData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SlotNumber = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FrontWeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BackWeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AvailableCoinCount = 0;
};

/** ReadyCoin UI부터 CoinBehaviorPhase의 CoinActor까지 이어지는 논리 코인 데이터입니다. */
USTRUCT(BlueprintType)
struct FReadyCoinData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinInstanceID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SourceSlotNumber = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FrontWeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BackWeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CurrentHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCanCancel = true;
};

/** PlayerController가 DB 데이터와 CoinManager 상태를 조합해 HUD에 전달하는 표시용 데이터입니다. */
USTRUCT(BlueprintType)
struct FBattleCoinSlotViewData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SlotNumber = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FrontWeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BackWeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* FrontIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* BackIcon = nullptr;
};

USTRUCT(BlueprintType)
struct FBattleReadyCoinViewData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ReadySlotNumber = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinInstanceID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CurrentHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCanCancel = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* FrontIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* BackIcon = nullptr;
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

