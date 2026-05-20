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
    AssignBossesToStages();
}



void UBossSetupGISubsystem::AssignBossesToStages()
{
    StageBossAssignment.Reset();

    // BossID 2~5를 Stage 1~4로 랜덤 배정 (튜토리얼 보스 BossID 1은 L_Stage_Battle_Tutorial 전용)
    TArray<int32> BossIDs;
    for (const FBossDisplayData& Data : AllBossData)
    {
        //임시수정
        if (Data.BossID == 2 || Data.BossID == 4)
            BossIDs.Add(Data.BossID);
    }

    // 셔플
    for (int32 i = BossIDs.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        BossIDs.Swap(i, j);
    }

    for (int32 i = 0; i < BossIDs.Num(); ++i)
    {
        StageBossAssignment.Add(i + 1, BossIDs[i]);
    }

    UE_LOG(LogTemp, Warning, TEXT("[BossSetupGI] Stage assignment:"));
    for (const TTuple<int32, int32>& Pair : StageBossAssignment)
    {
        UE_LOG(LogTemp, Warning, TEXT("  Stage %d → BossID %d"), Pair.Key, Pair.Value);
    }
}

bool UBossSetupGISubsystem::PrepareBossForStage(int32 StageIndex)
{
    ClearPreparedBoss();

    const int32* BossIDPtr = StageBossAssignment.Find(StageIndex);
    if (!BossIDPtr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossSetupGI] PrepareBossForStage failed: no boss assigned for stage %d"), StageIndex);
        return false;
    }

    const int32 BossID = *BossIDPtr;
    const FBossDisplayData* Found = AllBossData.FindByPredicate([BossID](const FBossDisplayData& D) { return D.BossID == BossID; });
    if (!Found)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossSetupGI] PrepareBossForStage failed: BossID %d not found"), BossID);
        return false;
    }

    PreparedBossData = *Found;
    PreparedContext.StageIndex = StageIndex;
    PreparedContext.PickedBossID = Found->BossID;
    PreparedContext.PickedBossName = Found->BossName;
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
    PreparedContext.StageIndex = 1;
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
