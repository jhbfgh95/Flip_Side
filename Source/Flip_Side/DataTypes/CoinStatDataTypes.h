#pragma once

#include "CoreMinimal.h"
#include "DataTypes/AttackAreaTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinStatDataTypes.generated.h"

struct FFaceData;

/** 상태효과가 현재 턴에만 남는지, 전투가 끝날 때까지 남는지 구분합니다. */
UENUM(BlueprintType)
enum class EBuffDurationType : uint8
{
	TurnOnly,
	PersistentInBattle
};

/** 동일한 상태효과 데이터가 버프인지 디버프인지 구분합니다. */
UENUM(BlueprintType)
enum class EStatusPolarity : uint8
{
	Buff,
	Debuff
};

/** 보스별 디버프 중첩 규칙을 선택하며, 버프는 항상 Stack을 사용합니다. */
UENUM(BlueprintType)
enum class EStatusStackPolicy : uint8
{
	Stack,
	NonStackable
};

/** 버프 아이콘과 테두리를 결정할 실제 적용 출처의 종류입니다. */
UENUM(BlueprintType)
enum class EStatusEffectSourceType : uint8
{
	None,
	Coin,
	Item,
	Boss
};

/** 단순 수치 Modifier 외에 피해 시점에 반응해야 하는 상태효과 동작입니다. */
UENUM(BlueprintType)
enum class EStatusReactiveBehavior : uint8
{
	None,
	DodgeChance,
	ReduceNextDamageAndGrantAttack,
	SurviveLethalOnce,
	TemporaryShield
};

/** 호버 UI와 행동 후보 스냅숏이 어느 값의 변경으로 무효화됐는지 알려줍니다. */
UENUM(BlueprintType, meta = (Bitflags))
enum class EWeaponStatChangeFlags : uint8
{
	None = 0,
	NumericStats = 1 << 0,
	AttackAreaSpec = 1 << 1,
	AbilityAreaSpec = 1 << 2,
	CurrentFace = 1 << 3,
	All = NumericStats | AttackAreaSpec | AbilityAreaSpec | CurrentFace
};
ENUM_CLASS_FLAGS(EWeaponStatChangeFlags);

/** 무기 정의, 버프 수정치, 최종 행동 스냅숏이 공통으로 사용하는 수치 스탯입니다. */
USTRUCT(BlueprintType)
struct FWeaponNumericStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponCnt = 1;
};

/** DB 무기 정의를 앞면 또는 뒷면의 불변 기본값으로 옮겨 담는 구조체입니다. */
USTRUCT(BlueprintType)
struct FWeaponFaceStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponID = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponNumericStats BaseNumericStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackAreaSpec AttackAreaSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackAreaSpec AbilityAreaSpec;

	// DB에 능력 사거리 None이 추가되기 전까지 공격 전용 무기와 능력 사거리 보유 무기를 구분합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasAbilityArea = false;
};

/** 범위형 버프가 공격·능력 규격의 ParamA/ParamB를 독립적으로 수정합니다. */
USTRUCT(BlueprintType)
struct FAttackAreaSpecModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ParamA = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ParamB = 0;

	bool IsZero() const
	{
		return ParamA == 0 && ParamB == 0;
	}
};

/** 버프·디버프 하나가 앞면과 뒷면에 공통으로 더하는 수치 수정치입니다. */
USTRUCT(BlueprintType)
struct FWeaponStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponCnt = 0;

	// 최대 HP 버프는 양면 무기 수치와 별개로 코인 공통 생존 스탯에 적용됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackAreaSpecModifier AttackAreaSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackAreaSpecModifier AbilityAreaSpec;

	bool IsZero() const
	{
		return AttackPoint == 0 && WeaponPoint == 0 && WeaponCnt == 0 && MaxHP == 0 &&
			AttackAreaSpec.IsZero() && AbilityAreaSpec.IsZero();
	}
};

/** 같은 BuffTypeID도 개별 제거할 수 있도록 고유 시리얼을 가진 런타임 상태효과입니다. */
USTRUCT(BlueprintType)
struct FStatusEffectInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BuffTypeID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BuffInstanceSerial = INDEX_NONE;

	// UI는 Coin이면 WeaponID, Item이면 ItemID인 SourceDataID로 기존 DataManager 아이콘을 조회합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusEffectSourceType SourceType = EStatusEffectSourceType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SourceDataID = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusPolarity Polarity = EStatusPolarity::Buff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuffDurationType DurationType = EBuffDurationType::TurnOnly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusStackPolicy StackPolicy = EStatusStackPolicy::Stack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RemainingTurns = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStatModifier Modifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusReactiveBehavior ReactiveBehavior = EStatusReactiveBehavior::None;

	// 확률, 피해 감소량 또는 최초 보호막 부여량처럼 반응 로직의 고정 수치입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReactiveMagnitude = 0;

	// 남은 임시 보호막처럼 실행 도중 감소하는 런타임 수치입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RuntimeValue = 0;

	// 0은 횟수 제한 없음이며, 1 이상이면 반응할 때마다 감소합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RemainingTriggers = 0;
};

/** 한 면의 기본값과 모든 활성 수정치를 합친 호버 표시용 계산 결과입니다. */
USTRUCT(BlueprintType)
struct FResolvedWeaponFaceStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 WeaponID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponNumericStats BaseNumericStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponStatModifier AppliedModifier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponNumericStats FinalNumericStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAttackAreaSpec AttackAreaSpec;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAttackAreaSpec AbilityAreaSpec;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasAbilityArea = false;
};

/** 클릭 시 UWeapon_Action으로 넘길 현재 면의 불변 행동 후보 스냅숏입니다. */
USTRUCT(BlueprintType)
struct FWeaponActionSnapshot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFaceState Face = EFaceState::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 WeaponID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponNumericStats FinalNumericStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAttackAreaSpec AttackAreaSpec;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAttackAreaSpec AbilityAreaSpec;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasAbilityArea = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SourceStatRevision = 0;
};

/** CoinActor를 턴 사이에 파괴해도 CoinManager가 보존해야 하는 전투 내 생존 상태입니다. */
USTRUCT(BlueprintType)
struct FCoinRuntimeStateSnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseMaxHP = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHP = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Shield = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStatusEffectInstance> PersistentStatusEffects;
};

/** CoinManager가 액터 생성 직후 StatComponent에 양면 정의와 생존 상태를 한 번에 주입합니다. */
USTRUCT(BlueprintType)
struct FCoinStatInitializeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponFaceStats FrontWeaponStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponFaceStats BackWeaponStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCoinRuntimeStateSnapshot RuntimeState;
};

/** StatComponent 리비전 이벤트의 변경 범위를 PlayerController 호버 처리에 전달합니다. */
USTRUCT(BlueprintType)
struct FWeaponStatsChangedEvent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Revision = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponStatChangeFlags ChangeFlags = EWeaponStatChangeFlags::None;
};

/** Actor VFX와 추후 상태 아이콘 UI가 특정 BuffTypeID의 현재 중첩 수를 받습니다. */
USTRUCT(BlueprintType)
struct FStatusEffectsChangedEvent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BuffTypeID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EStatusEffectSourceType SourceType = EStatusEffectSourceType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SourceDataID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TotalStackCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDebuff = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActive = false;
};

/** DataManager의 무기 정의를 전투 스냅숏이 사용하는 양면 기본 스탯으로 변환합니다. */
FLIP_SIDE_API FWeaponFaceStats BuildWeaponFaceStatsFromDefinition(const FFaceData& WeaponDefinition);
