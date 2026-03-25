#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "BossDataTypes.h"
#include "BossManagerSubsystem.generated.h"

class ABossActor;
class UBossPatternBase;
class ACoinActor;
class AGridActor;

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
};

USTRUCT(BlueprintType)
struct FBossStageContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 PickedThemeID = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 PickedBossID = 0;

    UPROPERTY(BlueprintReadOnly)
    FString PickedBossName;
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
    bool bPrepared = false;

    void Reset()
    {
        CurrentPattern = nullptr;
        CurrentPatternIndex = -1;
        LockedCells.Reset();
        LockedTargets.Reset();
        bPrepared = false;
    }
};

UCLASS()
class FLIP_SIDE_API UBossManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
    UPROPERTY()
    TObjectPtr<ABossActor> CurrentBoss = nullptr;

    UPROPERTY()
    FBossTurnContext TurnContext;

    UPROPERTY()
    FBossStageContext StageContext;

    FTimerHandle TelegraphTimerHandle;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool SpawnPreparedBoss();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    ABossActor* GetCurrentBoss() const { return CurrentBoss; }

    UFUNCTION(BlueprintCallable, Category = "Boss")
    const FBossStageContext& GetStageContext() const { return StageContext; }

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool StartBossSetting();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ExecuteCurrentPattern();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ClearCurrentTurn();

    void GetCoinsOnCells(const TArray<FGridPoint>& Cells, TArray<ACoinActor*>& OutCoins) const;
    void ApplyDamageToLockedTargets(const TArray<ACoinActor*>& LockedTargets, int32 Damage);

private:
    bool Internal_SpawnBoss(const FBossData& InBossData);

    UFUNCTION()
    void ApplyCurrentPattern();

    bool PrepareCurrentPattern();
    void ShowTelegraphPreview(const TArray<FGridPoint>& Cells, const FLinearColor& Color);
    void ClearTelegraphPreview(const TArray<FGridPoint>& Cells);
    bool IsCellIncluded(const FGridPoint& P, const TArray<FGridPoint>& Cells) const;
    void BuildLockedTargetsFromCells(const TArray<FGridPoint>& Cells, TArray<FLockedBossTarget>& OutLockedTargets) const;
    bool IsStillOnLockedCell(const FLockedBossTarget& LockedTarget) const;
};