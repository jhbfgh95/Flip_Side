// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipSide_Enum.generated.h"

/**
 * 무기 역할군
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
 * 코인(유닛)의 앞/뒤 상태
 */
UENUM(BlueprintType)
enum class EFaceState : uint8
{
    None  UMETA(DisplayName = "None"),
    Front UMETA(DisplayName = "Front"),
    Back  UMETA(DisplayName = "Back"),
};

/**
 * 턴 진행 상태
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
 * 그리드 점유 타입
 */
UENUM(BlueprintType)
enum class EGridOccupyingType : uint8
{
    None UMETA(DisplayName = "None"),
    Coin UMETA(DisplayName = "Coin"),
    Wall UMETA(DisplayName = "Wall"),
    Item UMETA(DisplayName = "Item"),
};