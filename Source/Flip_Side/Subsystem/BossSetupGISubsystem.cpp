#include "BossSetupGISubsystem.h"
#include "DataManagerSubsystem.h"
#include "Engine/GameInstance.h"

void UBossSetupGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UDataManagerSubsystem>();
    Super::Initialize(Collection);

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDataManagerSubsystem* DataMgr = GI->GetSubsystem<UDataManagerSubsystem>())
        {
            for (const TTuple<int32, FBossDisplayData>& Pair : DataMgr->BossByID)
            {
                AllBossData.Add(Pair.Value);
            }
        }
    }

    ClearPreparedBoss();
}


bool UBossSetupGISubsystem::PickRandomThemeFromStageBosses(
    const TArray<FBossDisplayData>& StageBosses,
    int32& OutThemeID) const
{
    TArray<int32> ThemeIDs;

    for (const FBossDisplayData& Data : StageBosses)
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
    const TArray<FBossDisplayData>& StageBosses,
    int32 ThemeID,
    FBossDisplayData& OutBossData) const
{
    TArray<FBossDisplayData> Candidates;

    for (const FBossDisplayData& Data : StageBosses)
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

bool UBossSetupGISubsystem::PickRandomBossDataForStage(int32 StageIndex, FBossDisplayData& OutBossData) const
{
    TArray<FBossDisplayData> StageBosses;

    for (const FBossDisplayData& Data : AllBossData)
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

    FBossDisplayData PickedBossData;

    if (!PickRandomBossDataForStage(StageIndex, PickedBossData))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossSetupGI] PrepareBossForStage failed: no boss for stage %d"), StageIndex);
        return false;
    }

    PreparedBossData = PickedBossData;
    PreparedContext.StageIndex = StageIndex;
    PreparedContext.PickedThemeID = PickedBossData.ThemeID;
    PreparedContext.PickedBossID = PickedBossData.BossID;
    PreparedContext.PickedBossName = PickedBossData.BossName;
    PreparedContext.bPrepared = true;

    return true;
}

bool UBossSetupGISubsystem::PrepareBossForID(int32 BossID)
{
    ClearPreparedBoss();

    const FBossDisplayData* Found = AllBossData.FindByPredicate([BossID](const FBossDisplayData& D) { return D.BossID == BossID; });
    if (!Found)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossSetupGI] PrepareBossForID failed: BossID %d not found"), BossID);
        return false;
    }

    PreparedBossData = *Found;
    PreparedContext.StageIndex = Found->BossStage;
    PreparedContext.PickedThemeID = Found->ThemeID;
    PreparedContext.PickedBossID = Found->BossID;
    PreparedContext.PickedBossName = Found->BossName;
    PreparedContext.bPrepared = true;

    return true;
}

bool UBossSetupGISubsystem::HasPreparedBoss() const
{
    return PreparedContext.bPrepared;
}

bool UBossSetupGISubsystem::GetPreparedBossData(FBossDisplayData& OutBossData) const
{
    if (!PreparedContext.bPrepared)
    {
        return false;
    }

    OutBossData = PreparedBossData;
    return true;
}

bool UBossSetupGISubsystem::GetPreparedBossInfo(FBossDisplayData& OutBossData) const
{
    return GetPreparedBossData(OutBossData);
}

void UBossSetupGISubsystem::ClearPreparedBoss()
{
    PreparedBossData = FBossDisplayData{};
    PreparedContext.Reset();
}
