#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "AttackAreaTypes.h"
#include "BossDataTypes.generated.h"

class ABossActor;
class UBossPatternBase;
class UBossGimmickBase;

UENUM(BlueprintType)
enum class EBossPatternTarget : uint8
{
    BossLocation,   // 미사용 (예약)
    TargetCells,    // 패턴 적용 그리드 각각에서 재생
    AllCells,       // 판떼기 중앙 고정 위치에서 재생
    AnchorCell,     // AnchorCell 좌표에서 재생 (원형/십자 등)
};

UENUM(BlueprintType)
enum class EBossGimmickType : uint8
{
    None       = 0,
    Shield     = 1,
    GridDebuff = 2,
    RoleTarget = 3,
    Groggy     = 4,
    Poison     = 5,
    Blind      = 6,
};

// 기믹 데이터
USTRUCT(BlueprintType)
struct FBossGimmickData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBossGimmickType GimmickType = EBossGimmickType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ParamIntA = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ParamIntB = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ParamFloatA = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ParamFloatB = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ParamFloatC = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ParamStrA;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ShieldValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GimmickName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GimmickDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GimmickClassPath;
};

// 상점용 - 가벼운 보스 표시 데이터
USTRUCT(BlueprintType)
struct FBossDisplayData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ThemeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BossName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> BossImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ShieldValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText BossAbilityDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FBossGimmickData> GimmickList;
};

// 상점용 패턴 표시 데이터
USTRUCT(BlueprintType)
struct FBossPatternDisplayData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString PatternName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText PatternDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DamageRatio = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> PatternIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> PatternRangeImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FAttackAreaSpec PatternSpec;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ShieldHeal = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBossGimmickType GimmickType = EBossGimmickType::None;

    // 실명/늪: 지속 턴수, 독: 독 데미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GimmickParamA = 0;

    // 늪: 공격력 디버프 수치
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GimmickParamB = 0;
};

// 배틀용 패턴 데이터
USTRUCT(BlueprintType)
struct FBossPatternBattleData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString PatternName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText PatternDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DamageRatio = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ApplyDelay = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FAttackAreaSpec PatternSpec;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UAnimMontage> PatternMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<class UNiagaraSystem> PatternEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBossPatternTarget PatternEffectTarget = EBossPatternTarget::BossLocation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector PatternScale = FVector::OneVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<class ABase_PatternVisualActor> VisualActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> PatternIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> PatternRangeImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bNoDamage = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBossGimmickType GimmickType = EBossGimmickType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ShieldHeal = 0;
};

// 전투 HUD 표시 전용 보스 상태 데이터입니다.
USTRUCT()
struct FBossHUDData
{
    GENERATED_BODY()

    FString BossName;
    int32 CurrentHP = 0;
    int32 MaxHP = 0;
    int32 CurrentShield = 0;
    int32 MaxShield = 0;

    // 그로기 보스는 실드 게임 데이터 대신 HUD의 실드 영역을 그로기 표시로 사용합니다.
    bool bUseGroggyAsShield = false;
    int32 CurrentGroggy = 0;
    int32 MaxGroggy = 0;

    bool bHasPatternInfo = false;
    int32 PatternDisplayIndex = INDEX_NONE;
    FString PatternName;
    FText PatternDescription;
    int32 PatternDamage = 0;
    TObjectPtr<UTexture2D> PatternIcon = nullptr;
};

// 배틀용 보스 전체 데이터
USTRUCT(BlueprintType)
struct FBossBattleData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ThemeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BossName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> BossImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ShieldValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector SpawnLoc = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator SpawnRot = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<ABossActor> BossClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UBossPatternBase> PatternClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float StageMultiplierStat = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float StageMultiplierGimmick = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> ClearAnim;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> HitAnim;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FBossPatternBattleData> PatternList;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FBossGimmickData> GimmickList;

    // slot 0=Right, 1=Bottom, 2=Left, 3=Front
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<UTexture2D>> BackgroundTextures;
};
