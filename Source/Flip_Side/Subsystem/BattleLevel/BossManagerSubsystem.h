#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "BossDataTypes.h"
#include "BossPhaseContext.h"

#include "BossManagerSubsystem.generated.h"

class ABossActor;
class ABossPillarActor;
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
    virtual void Deinitialize() override;

protected:
    UPROPERTY()
    TObjectPtr<ABossActor> CurrentBoss = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Boss|Pillars")
    TObjectPtr<ABossPillarActor> LeftPillar = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Boss|Pillars")
    TObjectPtr<ABossPillarActor> RightPillar = nullptr;

    FBossBattleData CurrentBossBattleData;

    UPROPERTY()
    FBossPhaseContext PhaseContext;

    UPROPERTY()
    FBossStageContext StageContext;

    FTimerHandle TelegraphTimerHandle;
    bool bAttackExecuting = false;
    bool bPatternApplied = false;
    bool bVisualActStarted = false;

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

    UBossPatternBase* GetCurrentPhasePattern() const { return PhaseContext.CurrentPattern; }
    int32 GetCurrentPhasePatternIndex() const { return PhaseContext.CurrentPatternIndex; }
    const TArray<FGridPoint>& GetCurrentPhaseLockedCells() const { return PhaseContext.LockedCells; }
    const FBossPhaseContext& GetPhaseContext() const { return PhaseContext; }

    void RecalculateTelegraphForRoleTarget();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool StartBossSetting();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ExecuteCurrentPattern();

    bool IsAttackExecuting() const { return bAttackExecuting; }
    void ApplyCurrentPattern();
    void PlayCurrentVisualAct();
    void FinishCurrentAttack(bool bInterrupted);

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ClearCurrentPhase();

    void BroadcastCoinLanded();

private:
    bool Internal_SpawnBoss(const FBossBattleData& InBossData);

    ABossPillarActor* SpawnBossPillar(TSubclassOf<ABossPillarActor> PillarClass, bool bLeft);
    bool PlaceBossPillar(ABossPillarActor* Pillar, bool bLeft);

    UFUNCTION()
    void DestroyBossPillars();

    UFUNCTION()
    void HandleBossDestroyed(AActor* DestroyedActor);

    bool PrepareCurrentPattern();
    void ShowTelegraphPreview(const TArray<FGridPoint>& Cells, const FLinearColor& Color);
    void ShowTelegraphPreviewWithSwamp(const TArray<FGridPoint>& Cells, const FLinearColor& Color);
    void ClearTelegraphPreview(const TArray<FGridPoint>& Cells);
    bool IsCellIncluded(const FGridPoint& P, const TArray<FGridPoint>& Cells) const;
    void BuildLockedTargetsFromCells(const TArray<FGridPoint>& Cells, TArray<FLockedBossTarget>& OutLockedTargets) const;
    bool IsStillOnLockedCell(const FLockedBossTarget& LockedTarget) const;
};
