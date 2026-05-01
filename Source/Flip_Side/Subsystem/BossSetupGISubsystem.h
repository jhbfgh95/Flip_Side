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
    int32 PickedThemeID = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 PickedBossID = 0;

    UPROPERTY(BlueprintReadOnly)
    FString PickedBossName;

    UPROPERTY(BlueprintReadOnly)
    bool bPrepared = false;

    void Reset()
    {
        StageIndex = -1;
        PickedThemeID = 0;
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

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    bool PickRandomThemeFromStageBosses(const TArray<FBossDisplayData>& StageBosses, int32& OutThemeID) const;
    bool PickRandomBossFromTheme(const TArray<FBossDisplayData>& StageBosses, int32 ThemeID, FBossDisplayData& OutBossData) const;
    bool PickRandomBossDataForStage(int32 StageIndex, FBossDisplayData& OutBossData) const;

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

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ClearPreparedBoss();
};
