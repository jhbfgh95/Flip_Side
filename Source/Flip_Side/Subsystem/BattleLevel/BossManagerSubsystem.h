#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "BossDataTypes.h"
#include "BossTurnContext.h"

#include "BossManagerSubsystem.generated.h"

class ABossActor;
class UBossPatternBase;
class UBossGimmickBase;
class ACoinActor;
class AGridActor;
class ABase_PatternVisualActor;
class ABase_OtherActor;

USTRUCT(BlueprintType)
struct FBossStageContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 PickedBossID = 0;

    UPROPERTY(BlueprintReadOnly)
    FString PickedBossName;
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

    FBossBattleData CurrentBossBattleData;

    UPROPERTY()
    FBossTurnContext TurnContext;

    UPROPERTY()
    FBossStageContext StageContext;

    FTimerHandle TelegraphTimerHandle;
    FTimerHandle ApplyPatternTimerHandle;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool SpawnPreparedBoss();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    ABossActor* GetCurrentBoss() const { return CurrentBoss; }

    UFUNCTION(BlueprintCallable, Category = "Boss")
    const FBossStageContext& GetStageContext() const { return StageContext; }

    UFUNCTION(BlueprintCallable, Category = "Boss")
    TSoftClassPtr<ABase_PatternVisualActor> GetCurrentPatternVisualClass() const;

    UBossPatternBase* GetCurrentTurnPattern() const { return TurnContext.CurrentPattern; }
    int32 GetCurrentTurnPatternIndex() const { return TurnContext.CurrentPatternIndex; }
    const TArray<FGridPoint>& GetCurrentTurnLockedCells() const { return TurnContext.LockedCells; }
    const FBossTurnContext& GetTurnContext() const { return TurnContext; }

    void RecalculateTelegraphForRoleTarget();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool StartBossSetting();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ExecuteCurrentPattern();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ClearCurrentTurn();

    void BroadcastCoinLanded();

private:
    bool Internal_SpawnBoss(const FBossBattleData& InBossData);

    UFUNCTION()
    void ApplyCurrentPattern();

    bool PrepareCurrentPattern();
    void ShowTelegraphPreview(const TArray<FGridPoint>& Cells, const FLinearColor& Color);
    void ShowTelegraphPreviewWithSwamp(const TArray<FGridPoint>& Cells, const FLinearColor& Color);
    void ClearTelegraphPreview(const TArray<FGridPoint>& Cells);
    bool IsCellIncluded(const FGridPoint& P, const TArray<FGridPoint>& Cells) const;
    void BuildLockedTargetsFromCells(const TArray<FGridPoint>& Cells, TArray<FLockedBossTarget>& OutLockedTargets) const;
    bool IsStillOnLockedCell(const FLockedBossTarget& LockedTarget) const;
};
