#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BossDataTypes.h"
#include "BossPatternBase.h"
#include "BossSetupGISubsystem.generated.h"

USTRUCT(BlueprintType)
struct FPreparedBossContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 StageIndex = -1;

    UPROPERTY(BlueprintReadOnly)
    int32 PickedBossID = 0;

    UPROPERTY(BlueprintReadOnly)
    FString PickedBossName;

    UPROPERTY(BlueprintReadOnly)
    bool bPrepared = false;

    void Reset()
    {
        StageIndex = -1;
        PickedBossID = 0;
        PickedBossName.Reset();
        bPrepared = false;
    }
};

UCLASS()
class FLIP_SIDE_API UBossSetupGISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<FBossDisplayData> AllBossData;

    UPROPERTY()
    FBossDisplayData PreparedBossData;

    UPROPERTY()
    FPreparedBossContext PreparedContext;

    // stage_id → boss_id 런타임 배정 결과
    TMap<int32, int32> StageBossAssignment;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void AssignBossesToStages();


public:
    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool PrepareBossForStage(int32 StageIndex);

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool PrepareBossForID(int32 BossID);

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool HasPreparedBoss() const;

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool GetPreparedBossData(FBossDisplayData& OutBossData) const;

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool GetPreparedBossInfo(FBossDisplayData& OutBossData) const;

    UFUNCTION(BlueprintCallable, Category = "Boss")
    const FPreparedBossContext& GetPreparedBossContext() const { return PreparedContext; }

    // 저장 시스템에 현재 게임에서 결정된 보스 등장 순서를 복사본으로 제공합니다.
    TMap<int32, int32> GetStageBossAssignments() const;

    // 저장된 보스 등장 순서를 복원합니다.
    bool SetStageBossAssignments(const TMap<int32, int32>& InAssignments);

    // 저장 시점의 스테이지와 보스 ID로 준비 상태를 정확히 복원합니다.
    bool PrepareBossForSavedData(int32 StageIndex, int32 BossID);

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ClearPreparedBoss();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ResetBossStageAssignments()
    {
        AssignBossesToStages();
    }
};
