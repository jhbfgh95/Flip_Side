#include "BossSetupGISubsystem.h"
#include "FlipSideDevloperSettings.h"

void UBossSetupGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (Settings)
    {
        TutorialBossData = Settings->TutorialBossData;
        AllBossData = Settings->AllBossData;
    }

    ClearPreparedBoss();

    //테스트용
    PrepareBossForStage(0);
}

bool UBossSetupGISubsystem::PickRandomThemeFromStageBosses(
    const TArray<FBossData>& StageBosses,
    int32& OutThemeID) const
{
    TArray<int32> ThemeIDs;

    for (const FBossData& Data : StageBosses)
    {
        if (!ThemeIDs.Contains(Data.ThemeID))
        {
            ThemeIDs.Add(Data.ThemeID);
        }
    }

    if (ThemeIDs.Num() <= 0)
    {
        return false;
    }

    const int32 PickedIndex = FMath::RandRange(0, ThemeIDs.Num() - 1);
    OutThemeID = ThemeIDs[PickedIndex];
    return true;
}

bool UBossSetupGISubsystem::PickRandomBossFromTheme(
    const TArray<FBossData>& StageBosses,
    int32 ThemeID,
    FBossData& OutBossData) const
{
    TArray<FBossData> Candidates;

    for (const FBossData& Data : StageBosses)
    {
        if (Data.ThemeID == ThemeID)
        {
            Candidates.Add(Data);
        }
    }

    if (Candidates.Num() <= 0)
    {
        return false;
    }

    const int32 PickedIndex = FMath::RandRange(0, Candidates.Num() - 1);
    OutBossData = Candidates[PickedIndex];
    return true;
}

bool UBossSetupGISubsystem::PickRandomBossDataForStage(int32 StageIndex, FBossData& OutBossData) const
{
    TArray<FBossData> StageBosses;

    for (const FBossData& Data : AllBossData)
    {
        if (Data.BossStage == StageIndex)
        {
            StageBosses.Add(Data);
        }
    }

    if (StageBosses.Num() <= 0)
    {
        return false;
    }

    int32 PickedThemeID = 0;
    if (!PickRandomThemeFromStageBosses(StageBosses, PickedThemeID))
    {
        return false;
    }

    return PickRandomBossFromTheme(StageBosses, PickedThemeID, OutBossData);
}

bool UBossSetupGISubsystem::PrepareBossForStage(int32 StageIndex)
{
    ClearPreparedBoss();

    FBossData PickedBossData;

    if (StageIndex == 0)
    {
        PickedBossData = TutorialBossData;
    }
    else
    {
        if (!PickRandomBossDataForStage(StageIndex, PickedBossData))
        {
            UE_LOG(LogTemp, Warning, TEXT("[BossSetupGI] PrepareBossForStage failed: no boss for stage %d"), StageIndex);
            return false;
        }
    }

    PreparedBossData = PickedBossData;
    PreparedContext.StageIndex = StageIndex;
    PreparedContext.PickedThemeID = PickedBossData.ThemeID;
    PreparedContext.PickedBossID = PickedBossData.BossID;
    PreparedContext.PickedBossName = PickedBossData.BossName;
    PreparedContext.bPrepared = true;

    return true;
}

bool UBossSetupGISubsystem::HasPreparedBoss() const
{
    return PreparedContext.bPrepared;
}

bool UBossSetupGISubsystem::GetPreparedBossData(FBossData& OutBossData) const
{
    if (!PreparedContext.bPrepared)
    {
        return false;
    }

    OutBossData = PreparedBossData;
    return true;
}

void UBossSetupGISubsystem::ClearPreparedBoss()
{
    PreparedBossData = FBossData{};
    PreparedContext.Reset();
}