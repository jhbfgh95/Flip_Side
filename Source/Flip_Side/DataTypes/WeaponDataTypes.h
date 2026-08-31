// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"
#include "AttackAreaTypes.h"
#include "Sound/SoundBase.h"
class UBehavior; // forward declaration
#include "WeaponDataTypes.generated.h"

/** 공격과 능력 로직이 실행되는 순서를 Registry가 선언할 때 사용합니다. */
UENUM(BlueprintType)
enum class EAbilityTiming : uint8
{
    BeforeAttack,
    OnHit,
    AfterAttackAlways
};

/** [연속]이 공격과 능력 중 어느 채널을 반복하는지 구분합니다. */
UENUM(BlueprintType)
enum class ERepeatLogicChannel : uint8
{
    None,
    Attack,
    Ability
};

/** 반복 횟수를 불변 행동 스냅숏의 어느 수치에서 가져오는지 구분합니다. */
UENUM(BlueprintType)
enum class ERepeatCountSource : uint8
{
    One,
    WeaponPoint,
    WeaponCount
};

/** 능력 후보를 범위에서 어떻게 확정할지 정의합니다. */
UENUM(BlueprintType)
enum class EAbilitySelectionMode : uint8
{
    None,
    AllInRange,
    Single,
    UpToRepeatCount
};

/** 능력 범위에서 수집할 안정적인 대상 대분류입니다. */
UENUM(BlueprintType, meta = (Bitflags))
enum class EAbilityTargetFlags : uint8
{
    None = 0,
    Coin = 1 << 0,
    Obstacle = 1 << 1,
    Installation = 1 << 2,
    EmptyGrid = 1 << 3
};
ENUM_CLASS_FLAGS(EAbilityTargetFlags);

/** Registry가 능력별 수집 대상과 선택 방식을 CoinActionManager에 전달합니다. */
USTRUCT(BlueprintType)
struct FAbilityTargetRule
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Flip_Side.EAbilityTargetFlags"))
    int32 TargetFlags = static_cast<int32>(EAbilityTargetFlags::None);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAbilitySelectionMode SelectionMode = EAbilitySelectionMode::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERepeatCountSource RepeatCountSource = ERepeatCountSource::One;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bExcludeCaster = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCoinFieldOnly = false;

    bool HasTarget(EAbilityTargetFlags Flag) const
    {
        return (TargetFlags & static_cast<int32>(Flag)) != 0;
    }
};

/** 실제 보스 공격 한 번의 결과이며 보호막과 HP 피해를 분리해 [적중]·[타격]이 공유합니다. */
USTRUCT(BlueprintType)
struct FWeaponAttackResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bAttackAttempted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bEnemyInRange = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 RequestedDamage = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 ShieldDamage = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 HPDamage = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class ABossActor> Boss = nullptr;

    int32 GetTotalDamage() const
    {
        return ShieldDamage + HPDamage;
    }

    bool DidHit() const
    {
        return GetTotalDamage() > 0;
    }
};

/** 한 번의 코인 행동 동안 AttackLibrary와 AbilityLibrary가 공유하는 가변 실행 상태입니다. */
USTRUCT(BlueprintType)
struct FWeaponExecutionState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FWeaponAttackResult LastAttack;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 TotalDamageDealt = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 AbsorbedAmount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 StrikeAmount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CurrentRepeatIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<int32> SelectedCoinInstanceIDs;
};

/** 전투 무기 버프의 중첩·제거·UI 그룹화를 위해 C++에서 관리하는 정식 런타임 ID입니다. */
namespace WeaponBuffTypeID
{
    inline constexpr int32 BurgerStack = 10001;
    inline constexpr int32 Absorb = 10002;
    inline constexpr int32 Strike = 10003;
    inline constexpr int32 SmokeDodge = 10004;
    inline constexpr int32 ArmorGuard = 10005;
    inline constexpr int32 ArmorNextTurnAttack = 10006;
    inline constexpr int32 TemporaryShield = 10007;
    inline constexpr int32 AdrenalineAttack = 10008;
    inline constexpr int32 AmplificationRange = 10009;
    inline constexpr int32 EmergencySurvival = 10010;
    inline constexpr int32 CortisolWeaponPoint = 10011;
}

USTRUCT(BlueprintType)
struct FFaceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WeaponID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TypeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponClass WeaponType = EWeaponClass::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BehaviorPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGridPoint AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* WeaponIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BehaviorCode;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString KOR_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ENG_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UNiagaraSystem* WeaponVFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponVFXTarget WeaponVFXTarget = EWeaponVFXTarget::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FAttackAreaSpec AttackAreaSpec;

    // DB 능력 사거리 API가 연결되기 전까지 CoinManager의 테스트 데이터가 채웁니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FAttackAreaSpec AbilityAreaSpec;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FIntPoint AttackAnchorOffset = FIntPoint(0, 0);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackAreaFlags = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EActionRepeatType ActionRepeatType = EActionRepeatType::None;

    // pattern == SingleCell 일 때만 유효 (param_b 값)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ESingleCellWeaponType SingleCellWeaponType = ESingleCellWeaponType::Other;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* WeaponSFX = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponType
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TypeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponClass WeaponType = EWeaponClass::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor;
};

USTRUCT(BlueprintType)
struct FWeaponIDArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<int32> WeaponIDs;
};


/**
 * �ൿ �Ͽ��� ����� �۾� ����
 */
USTRUCT(BlueprintType)
struct FActionTask
{
    GENERATED_BODY()

    FGridPoint ModifiedRange;

    int32 ModifiedAttackPoint = 0;

    int32 ModifiedBehaviorPoint = 0;
};

