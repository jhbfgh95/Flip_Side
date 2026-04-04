#include "BossManagerSubsystem.h"

#include "BossActor.h"
#include "BossPatternBase.h"
#include "GridManagerSubsystem.h"
#include "CoinActor.h"
#include "Component_Status.h"
#include "GridActor.h"
#include "BossSetupGISubsystem.h"

#include "Engine/World.h"
#include "TimerManager.h"

void UBossManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UBossManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
    {
        return false;
    }

    UWorld* World = Cast<UWorld>(Outer);

    if (World)
    {
        FString MapName = World->GetName();
        if (MapName.Contains(TEXT("L_Stage")))
        {
            return true;
        }
    }

    return false;
}

bool UBossManagerSubsystem::SpawnPreparedBoss()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    if (IsValid(CurrentBoss))
    {
        CurrentBoss->Destroy();
        CurrentBoss = nullptr;
    }

    UGameInstance* GI = World->GetGameInstance();
    if (!GI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] SpawnPreparedBoss failed: GI null"));
        return false;
    }

    UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
    if (!BossSetupGI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] SpawnPreparedBoss failed: BossSetupGI null"));
        return false;
    }

    FBossData PreparedBossData;
    if (!BossSetupGI->GetPreparedBossData(PreparedBossData))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] SpawnPreparedBoss failed: no prepared boss data"));
        return false;
    }

    return Internal_SpawnBoss(PreparedBossData);
}

bool UBossManagerSubsystem::Internal_SpawnBoss(const FBossData& InBossData)
{
    UWorld* World = GetWorld();
    if (!World || InBossData.BossClass.IsNull())
    {
        return false;
    }

    UClass* SelectedBossClass = InBossData.BossClass.LoadSynchronous();
    if (!SelectedBossClass)
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABossActor* SpawnedBoss = World->SpawnActor<ABossActor>(
        SelectedBossClass,
        InBossData.SpawnLoc,
        InBossData.SpawnRot,
        SpawnParams
    );

    if (!SpawnedBoss)
    {
        return false;
    }

    CurrentBoss = SpawnedBoss;
    CurrentBoss->InitializeFromBossData(InBossData);

    StageContext.PickedThemeID = InBossData.ThemeID;
    StageContext.PickedBossID = InBossData.BossID;
    StageContext.PickedBossName = InBossData.BossName;

    return true;
}

bool UBossManagerSubsystem::StartBossSetting()
{
    if (!IsValid(CurrentBoss))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] StartBossSetting failed: CurrentBoss null"));
        return false;
    }

    if (!PrepareCurrentPattern())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] StartBossSetting failed: PrepareCurrentPattern failed"));
        return false;
    }

    CurrentBoss->PlayTelegraph();

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    return true;
}

bool UBossManagerSubsystem::PrepareCurrentPattern()
{
    ClearCurrentTurn();

    if (!IsValid(CurrentBoss))
    {
        return false;
    }

    const int32 PatternCount = CurrentBoss->GetPatternCount();
    if (PatternCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] PrepareCurrentPattern failed: no patterns"));
        return false;
    }

    const int32 PatternIndex = FMath::RandRange(0, PatternCount - 1);
    UBossPatternBase* PickedPattern = CurrentBoss->GetPattern();
    if (!PickedPattern)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] PrepareCurrentPattern failed: picked pattern null"));
        return false;
    }

    TurnContext.CurrentPattern = PickedPattern;
    TurnContext.CurrentPatternIndex = PatternIndex;
    TurnContext.CurrentPattern->BuildTargetCells(this, CurrentBoss, TurnContext.LockedCells, TurnContext.CurrentPatternIndex);

    if (PickedPattern->PatternData.IsValidIndex(TurnContext.CurrentPatternIndex))
    {
        CurrentBoss->SetPatternAnim(PickedPattern->PatternData[TurnContext.CurrentPatternIndex].PatternMontage);
    }

    BuildLockedTargetsFromCells(TurnContext.LockedCells, TurnContext.LockedTargets);
    ShowTelegraphPreview(TurnContext.LockedCells, FLinearColor(1.f, 0.f, 0.f, 1.f));

    TurnContext.bPrepared = true;
    return true;
}

void UBossManagerSubsystem::ExecuteCurrentPattern()
{
    if (!TurnContext.bPrepared)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] ExecuteCurrentPattern skipped: not prepared"));
        return;
    }

    if (!IsValid(CurrentBoss))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] ExecuteCurrentPattern skipped: CurrentBoss null"));
        ClearCurrentTurn();
        return;
    }
    
    CurrentBoss->PlayAttack();

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    float ApplyDelay = 1.0f;
    if (TurnContext.CurrentPattern && TurnContext.CurrentPattern->PatternData.IsValidIndex(TurnContext.CurrentPatternIndex))
    {
        ApplyDelay = TurnContext.CurrentPattern->PatternData[TurnContext.CurrentPatternIndex].AttackApplyDelay;
    }

    World->GetTimerManager().SetTimer(
        ApplyPatternTimerHandle,
        this,
        &UBossManagerSubsystem::ApplyCurrentPattern,
        ApplyDelay,
        false
    );
}

void UBossManagerSubsystem::ApplyCurrentPattern()
{
    if (!TurnContext.bPrepared)
    {
        return;
    }

    TArray<ACoinActor*> ValidLockedTargets;
    for (const FLockedBossTarget& LockedTarget : TurnContext.LockedTargets)
    {
        if (!IsValid(LockedTarget.CoinActor))
        {
            continue;
        }

        if (IsStillOnLockedCell(LockedTarget))
        {
            ValidLockedTargets.Add(LockedTarget.CoinActor);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[BossManager] CoinID=%d evaded from locked cell (%d,%d)"),
                LockedTarget.CoinID,
                LockedTarget.LockedGrid.GridX,
                LockedTarget.LockedGrid.GridY);
        }
    }

    if (TurnContext.CurrentPattern)
    {
        TurnContext.CurrentPattern->ExecutePattern(
            this,
            CurrentBoss,
            TurnContext.LockedCells,
            ValidLockedTargets,
            TurnContext.CurrentPatternIndex
        );
    }

    ClearCurrentTurn();
}

void UBossManagerSubsystem::ClearCurrentTurn()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TelegraphTimerHandle);
        World->GetTimerManager().ClearTimer(ApplyPatternTimerHandle);
    }

    if (TurnContext.LockedCells.Num() > 0)
    {
        ClearTelegraphPreview(TurnContext.LockedCells);
    }

    TurnContext.Reset();
}

void UBossManagerSubsystem::GetCoinsOnCells(const TArray<FGridPoint>& Cells, TArray<ACoinActor*>& OutCoins) const
{
    OutCoins.Reset();

    UGridManagerSubsystem* GridMgr = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if (!GridMgr)
    {
        return;
    }

    TArray<FCoinOnGridInfo> OccupiedCoins;
    GridMgr->CollectOccupiedCoins(OccupiedCoins);

    for (const FCoinOnGridInfo& Info : OccupiedCoins)
    {
        if (!IsValid(Info.CoinActor))
        {
            continue;
        }

        if (IsCellIncluded(Info.GridXY, Cells))
        {
            OutCoins.Add(Info.CoinActor);
        }
    }
}

void UBossManagerSubsystem::ApplyDamageToLockedTargets(const TArray<ACoinActor*>& LockedTargets, int32 Damage)
{
    for (ACoinActor* Coin : LockedTargets)
    {
        if (!IsValid(Coin))
        {
            continue;
        }

        UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
        if (!StatusComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("[BossManager] CoinID=%d has no Component_Status"),
                Coin->GetCoinID());
            continue;
        }

        const int32 PrevHP = StatusComp->GetHP();
        const int32 NextHP = PrevHP - FMath::Max(0, Damage);

        StatusComp->ApplyDamage(Damage, CurrentBoss);

        UE_LOG(LogTemp, Log, TEXT("[BossManager] Damage Applied - CoinID=%d HP %d -> %d"),
            Coin->GetCoinID(),
            PrevHP,
            FMath::Max(0, NextHP));
    }
}

void UBossManagerSubsystem::ShowTelegraphPreview(const TArray<FGridPoint>& Cells, const FLinearColor& Color)
{
    UGridManagerSubsystem* GridMgr = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if (!GridMgr)
    {
        return;
    }

    for (const FGridPoint& Cell : Cells)
    {
        if (AGridActor* Grid = GridMgr->GetGridActor(Cell))
        {
            Grid->bIsBossAttack = true;
            Grid->ApplyCellMaterialParams(Color, 0.9f, 0.0f);
        }
    }
}

void UBossManagerSubsystem::ClearTelegraphPreview(const TArray<FGridPoint>& Cells)
{
    UGridManagerSubsystem* GridMgr = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if (!GridMgr)
    {
        return;
    }

    for (const FGridPoint& Cell : Cells)
    {
        if (AGridActor* Grid = GridMgr->GetGridActor(Cell))
        {
            Grid->InitColor();
        }
    }
}

bool UBossManagerSubsystem::IsCellIncluded(const FGridPoint& P, const TArray<FGridPoint>& Cells) const
{
    for (const FGridPoint& Cell : Cells)
    {
        if (Cell.GridX == P.GridX && Cell.GridY == P.GridY)
        {
            return true;
        }
    }

    return false;
}

void UBossManagerSubsystem::BuildLockedTargetsFromCells(
    const TArray<FGridPoint>& Cells,
    TArray<FLockedBossTarget>& OutLockedTargets) const
{
    OutLockedTargets.Reset();

    UGridManagerSubsystem* GridMgr = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if (!GridMgr)
    {
        return;
    }

    TArray<FCoinOnGridInfo> OccupiedCoins;
    GridMgr->CollectOccupiedCoins(OccupiedCoins);

    for (const FCoinOnGridInfo& Info : OccupiedCoins)
    {
        if (!IsValid(Info.CoinActor))
        {
            continue;
        }

        if (!IsCellIncluded(Info.GridXY, Cells))
        {
            continue;
        }

        FLockedBossTarget NewTarget;
        NewTarget.CoinID = Info.CoinID;
        NewTarget.LockedGrid = Info.GridXY;
        NewTarget.CoinActor = Info.CoinActor;

        OutLockedTargets.Add(NewTarget);
    }
}

bool UBossManagerSubsystem::IsStillOnLockedCell(const FLockedBossTarget& LockedTarget) const
{
    if (!IsValid(LockedTarget.CoinActor))
    {
        return false;
    }

    const FGridPoint CurrentGrid = LockedTarget.CoinActor->GetDecidedGrid();

    return CurrentGrid.GridX == LockedTarget.LockedGrid.GridX
        && CurrentGrid.GridY == LockedTarget.LockedGrid.GridY;
}