#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "BossTurnContext.generated.h"

class UBossPatternBase;
class ACoinActor;
class ABase_OtherActor;

USTRUCT(BlueprintType)
struct FLockedBossTarget
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 CoinID = 0;

    UPROPERTY(BlueprintReadOnly)
    FGridPoint LockedGrid;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ACoinActor> CoinActor = nullptr;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ABase_OtherActor> OtherActor = nullptr;
};

USTRUCT(BlueprintType)
struct FBossTurnContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBossPatternBase> CurrentPattern = nullptr;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPatternIndex = -1;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGridPoint> LockedCells;

    UPROPERTY(BlueprintReadOnly)
    TArray<FLockedBossTarget> LockedTargets;

    UPROPERTY(BlueprintReadOnly)
    int32 BaseDamage = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 BonusDamage = 0;

    UPROPERTY(BlueprintReadOnly)
    float DamageMultiplier = 1.f;

    UPROPERTY(BlueprintReadOnly)
    bool bSkipAttack = false;

    UPROPERTY(BlueprintReadOnly)
    bool bPrepared = false;

    void Reset()
    {
        CurrentPattern = nullptr;
        CurrentPatternIndex = -1;
        LockedCells.Reset();
        LockedTargets.Reset();
        BaseDamage = 0;
        BonusDamage = 0;
        DamageMultiplier = 1.f;
        bSkipAttack = false;
        bPrepared = false;
    }
};
