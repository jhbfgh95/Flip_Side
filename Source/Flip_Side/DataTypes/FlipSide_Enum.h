// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipSide_Enum.generated.h"

/**
 * ���� ���ұ�
 */
UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
    None UMETA(DisplayName = "None"),
    Tank UMETA(DisplayName = "Tank"),
    Deal UMETA(DisplayName = "Deal"),
    Heal UMETA(DisplayName = "Heal"),
};

/**
 * ����(����)�� ��/�� ����
 */
UENUM(BlueprintType)
enum class EFaceState : uint8
{
    None  UMETA(DisplayName = "None"),
    Front UMETA(DisplayName = "Front"),
    Back  UMETA(DisplayName = "Back"),
};

/**
 * �� ���� ����
 */
UENUM(BlueprintType)
enum class ETurnState : uint8
{
    None           UMETA(DisplayName = "None"),
    CoinReadyTurn  UMETA(DisplayName = "Coin Ready Turn"),
    CoinSelectTurn UMETA(DisplayName = "Coin Select Turn"),
    BehaviorTurn   UMETA(DisplayName = "Behavior Turn"),
    BossTurn       UMETA(DisplayName = "Boss Turn"), 
    SettingTurn       UMETA(DisplayName = "Setting Turn")
};

/**
 * �׸��� ���� Ÿ��
 */
UENUM(BlueprintType)
enum class EGridOccupyingType : uint8
{
    None UMETA(DisplayName = "None"),
    Coin UMETA(DisplayName = "Coin"),
    Wall UMETA(DisplayName = "Wall"),
    Item UMETA(DisplayName = "Item"),
};

UENUM(BlueprintType)
enum class ECCTypes : uint8
{
    None UMETA(DisplayName = "None"),
    Stun UMETA(DisplayName = "Stun"),
    Sleep UMETA(DisplayName = "Sleep")
};

UENUM(BlueprintType)
enum class EActionInputState : uint8
{
    None,
    WaitingForCoinClick, //코인 클릭 기다리기
    ExecutingAction, //바로 실행
    WaitingForGridClick,
    WaitingForGridClickForItem //아이템 입력받고, 그리드 입력 기다림
};

UENUM(BlueprintType)
enum class EActionRepeatType : uint8
{
    None,
    Fix,
    Behavior,
    Attack
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Buff    UMETA(DisplayName = "버프형"),   // DB item_type_id = 1
    Install UMETA(DisplayName = "설치형"),   // DB item_type_id = 2
};

/**
 * 싱글셀 무기 전용 타입 (coin_weapon_attack_area.param_b 값으로 저장됨)
 */
UENUM(BlueprintType)
enum class ESingleCellWeaponType : uint8
{
    Coin UMETA(DisplayName = "Coin"),   // 0
    Grid UMETA(DisplayName = "Grid"),   // 1
    Other UMETA(DisplayName = "Other")
};