// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinStatDataTypes.h"
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

    // 슬롯 레벨(1~3). 이 값으로 구매 비용/HP를 조회함 (DataManagerSubsystem::GetCoinSlotLevelStats)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Level = 1;
};

/** DB 연결 전 코인 슬롯의 기본 무기 스탯을 표시하기 위한 데이터입니다. */
USTRUCT(BlueprintType)
struct FWeaponStatDisplayData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPower = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WeaponPower = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 0;
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

    /** DB 연결 전 UI 검증을 위해 CreateTestCoinSlots에서만 채우는 임시 데이터입니다. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FWeaponStatDisplayData FrontWeaponStats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FWeaponStatDisplayData BackWeaponStats;
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

    // CoinManager가 다음 CoinBehaviorPhase의 CoinActor 초기화에 사용할 버프 전 최대 HP입니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BaseMaxHP = 0;

    // CoinManager가 Actor 파괴 전 저장하고 다음 턴에 복원할 보호막입니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Shield = 0;

    // 턴 종료 시 TurnOnly를 제외하고 전투 안에서만 유지할 상태효과입니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FStatusEffectInstance> PersistentStatusEffects;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText FrontWeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText BackWeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FWeaponStatDisplayData FrontWeaponStats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FWeaponStatDisplayData BackWeaponStats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor FrontWeaponColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor BackWeaponColor = FLinearColor::White;
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

/** W_BattleCoinInfo에서 앞면 또는 뒷면 하나를 표시하기 위한 데이터입니다. */
USTRUCT(BlueprintType)
struct FBattleWeaponFaceInfoViewData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WeaponID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* WeaponIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText WeaponDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FWeaponNumericStats BaseStats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FWeaponNumericStats FinalStats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor WeaponColor = FLinearColor::White;
};

/** BattleBuffIconWidget 하나가 표시하는 집계된 상태효과 데이터입니다. */
USTRUCT(BlueprintType)
struct FBattleStatusEffectViewData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BuffTypeID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EStatusEffectSourceType SourceType = EStatusEffectSourceType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SourceDataID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EStatusPolarity Polarity = EStatusPolarity::Buff;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 StackCount = 1;
};

/** ReadyCoinSlot과 필드 CoinActor가 공통으로 W_BattleCoinInfo에 전달하는 양면 정보입니다. */
USTRUCT(BlueprintType)
struct FBattleCoinInfoViewData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinInstanceID = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FBattleWeaponFaceInfoViewData FrontFace;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FBattleWeaponFaceInfoViewData BackFace;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CurrentHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Shield = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FBattleStatusEffectViewData> StatusEffects;
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

