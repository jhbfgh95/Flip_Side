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
enum class EPhaseState : uint8
{
    None           UMETA(DisplayName = "None"),
    CoinReadyPhase  UMETA(DisplayName = "Coin Ready Phase"),
    CoinBehaviorPhase UMETA(DisplayName = "Coin Behavior Phase"),
    BehaviorPhase   UMETA(DisplayName = "Behavior Phase"),
    BossPhase       UMETA(DisplayName = "Boss Phase"), 
    SettingPhase       UMETA(DisplayName = "Setting Phase")
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
    Turret UMETA(DisplayName = "Item"),
};

UENUM(BlueprintType)
enum class ECCTypes : uint8
{
    None  UMETA(DisplayName = "None"),
    Stun  UMETA(DisplayName = "Stun"),
    Sleep UMETA(DisplayName = "Sleep"),
    Blind  UMETA(DisplayName = "Blind"),
    Poison UMETA(DisplayName = "Poison")
};

//코인 액션 기준
UENUM(BlueprintType)
enum class EActionInputState : uint8
{
    None,
    WaitingForCoinClick, //코인 클릭 기다리기
    ExecutingAction, //바로 실행
    WaitingForGridClick,
    WaitingForOtherClick,
    WaitingForItemExecute //아이템이 코인에 적용되기 위해서 기다림.
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
    CoinBuff    UMETA(DisplayName = "코인 버프형"),   // DB item_type_id = 1
    Install UMETA(DisplayName = "설치형"),   // DB item_type_id = 2
    OtherBuff UMETA(DisplayName = "설치 버프") 
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

/**
 * 무기 VFX가 적용될 대상 (coin_weapon_def.vfx_target 값으로 저장됨)
 */
UENUM(BlueprintType)
enum class EWeaponVFXTarget : uint8
{
    None          UMETA(DisplayName = "None"),           // 0
    Caster        UMETA(DisplayName = "Caster"),         // 1
    TargetGrid    UMETA(DisplayName = "TargetGrid"),     // 2
    TargetCoin    UMETA(DisplayName = "TargetCoin"),     // 3
    TargetOther   UMETA(DisplayName = "TargetOther"),    // 4
    Boss          UMETA(DisplayName = "Boss"),           // 5
    RangeCells    UMETA(DisplayName = "RangeCells"),     // 6
    AffectedCoins UMETA(DisplayName = "AffectedCoins")  // 7
};
