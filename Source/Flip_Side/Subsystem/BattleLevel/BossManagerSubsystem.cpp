#include "BossManagerSubsystem.h"

#include "BossActor.h"
#include "BossPillarActor.h"
#include "BattleLevelActingWSubsystem.h"
#include "W_BossHP.h"
#include "BossPatternBase.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Proxy.h"
#include "GridManagerSubsystem.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "Base_PatternVisualActor.h"
#include "BossSetupGISubsystem.h"
#include "DataManagerSubsystem.h"
#include "Actors/Others/Base_OtherActor.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "FlipSideDevloperSettings.h"

void UBossManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Collection.InitializeDependency<UGridManagerSubsystem>();

    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (GI)
    {
        UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
        if (BossSetupGI && !BossSetupGI->HasPreparedBoss())
        {
            const UFlipSideDevloperSettings* DevSettings = GetDefault<UFlipSideDevloperSettings>();
            const int32 FallbackID = (DevSettings && DevSettings->DebugForceBossID > 0)
                ? DevSettings->DebugForceBossID : 1;
            BossSetupGI->PrepareBossForID(FallbackID);
            UE_LOG(LogTemp, Warning, TEXT("[BossManager] No prepared boss, defaulting to BossID %d"), FallbackID);
        }
    }
}

void UBossManagerSubsystem::Deinitialize()
{
    DestroyBossPillars();
    if (IsValid(CurrentBoss))
    {
        CurrentBoss->OnBossDead.RemoveDynamic(this, &UBossManagerSubsystem::DestroyBossPillars);
        CurrentBoss->OnDestroyed.RemoveDynamic(this, &UBossManagerSubsystem::HandleBossDestroyed);
    }
    Super::Deinitialize();
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

    DestroyBossPillars();
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

    FBossDisplayData PreparedBossData;
    if (!BossSetupGI->GetPreparedBossData(PreparedBossData))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] SpawnPreparedBoss failed: no prepared boss data"));
        return false;
    }

    UDataManagerSubsystem* DataMgr = GI->GetSubsystem<UDataManagerSubsystem>();
    if (!DataMgr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] SpawnPreparedBoss failed: DataMgr null"));
        return false;
    }

    CurrentBossBattleData = FBossBattleData{};
    if (!DataMgr->LoadBossBattleData(PreparedBossData.BossID, CurrentBossBattleData))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] SpawnPreparedBoss failed: LoadBossBattleData failed"));
        return false;
    }

    const int32 StageIndex = BossSetupGI->GetPreparedBossContext().StageIndex;
    if (StageIndex > 0)
    {
        float StatMult = 1.0f, GimmickMult = 1.0f;
        if (DataMgr->TryGetStageMultiplier(PreparedBossData.BossID, StageIndex, StatMult, GimmickMult))
        {
            CurrentBossBattleData.StageMultiplierStat    = StatMult;
            CurrentBossBattleData.StageMultiplierGimmick = GimmickMult;
        }
    }

    return Internal_SpawnBoss(CurrentBossBattleData);
}

bool UBossManagerSubsystem::Internal_SpawnBoss(const FBossBattleData& InBossData)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] Internal_SpawnBoss: World null"));
        return false;
    }
    if (InBossData.BossClass.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] Internal_SpawnBoss: BossClass is null (BossID=%d)"), InBossData.BossID);
        return false;
    }

    UClass* SelectedBossClass = InBossData.BossClass.LoadSynchronous();
    if (!SelectedBossClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] Internal_SpawnBoss: BossClass LoadSynchronous failed"));
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
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] Internal_SpawnBoss: SpawnActor failed (BossID=%d, Class=%s)"),
            InBossData.BossID, *SelectedBossClass->GetName());
        return false;
    }

    CurrentBoss = SpawnedBoss;
    CurrentBoss->InitializeFromBossData(InBossData);

    if (UGridManagerSubsystem* GridManager = World->GetSubsystem<UGridManagerSubsystem>(); IsValid(GridManager))
    {
        GridManager->SetBossIcon(CurrentBoss, InBossData.BossIcon.Get());
    }

    if (InBossData.StageMultiplierStat != 1.0f)
    {
        const int32 ScaledHP = FMath::RoundToInt(InBossData.BossHP * InBossData.StageMultiplierStat);
        CurrentBoss->SetMaxHP(ScaledHP);
    }

    if (InBossData.ShieldValue > 0)
    {
        CurrentBoss->InitShield(InBossData.ShieldValue);
    }

    if (InBossData.PatternClass)
    {
        UBossPatternBase* NewPattern = NewObject<UBossPatternBase>(CurrentBoss, InBossData.PatternClass);
        if (NewPattern)
        {
            NewPattern->PatternData = InBossData.PatternList;
            CurrentBoss->SetPattern(NewPattern);
        }
    }

    for (const FBossGimmickData& GimmickData : InBossData.GimmickList)
    {
        TSubclassOf<UBossGimmickBase> GimmickClass = nullptr;
        if (!GimmickData.GimmickClassPath.IsEmpty())
        {
            UClass* Loaded = Cast<UClass>(FSoftObjectPath(GimmickData.GimmickClassPath).TryLoad());
            if (Loaded && Loaded->IsChildOf(UBossGimmickBase::StaticClass()))
                GimmickClass = Loaded;
            else
                UE_LOG(LogTemp, Warning, TEXT("[BossManager] GimmickClassPath load failed: %s"), *GimmickData.GimmickClassPath);
        }

        if (GimmickClass)
        {
            UBossGimmickBase* NewGimmick = NewObject<UBossGimmickBase>(CurrentBoss, GimmickClass.Get());
            if (NewGimmick)
            {
                NewGimmick->GimmickData = GimmickData;
                CurrentBoss->AddGimmick(NewGimmick);
                NewGimmick->OnBattleStart(CurrentBoss);
            }
        }
    }

    StageContext.PickedBossID = InBossData.BossID;
    StageContext.PickedBossName = InBossData.BossName;

    CurrentBoss->OnBossDead.AddUniqueDynamic(this, &UBossManagerSubsystem::DestroyBossPillars);
    CurrentBoss->OnDestroyed.AddUniqueDynamic(this, &UBossManagerSubsystem::HandleBossDestroyed);
    LeftPillar = SpawnBossPillar(CurrentBoss->GetLeftPillarClass(), true);
    RightPillar = SpawnBossPillar(CurrentBoss->GetRightPillarClass(), false);

    return true;
}

ABossPillarActor* UBossManagerSubsystem::SpawnBossPillar(TSubclassOf<ABossPillarActor> PillarClass, bool bLeft)
{
    UWorld* World = GetWorld();
    if (!IsValid(World) || !IsValid(CurrentBoss)) return nullptr;

    UClass* SpawnClass = PillarClass ? PillarClass.Get() : ABossPillarActor::StaticClass();
    ABossPillarActor* Pillar = World->SpawnActorDeferred<ABossPillarActor>(SpawnClass,
        FTransform::Identity, CurrentBoss, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!IsValid(Pillar)) return nullptr;

    Pillar->InitializeForBoss(CurrentBoss);
    if (!PlaceBossPillar(Pillar, bLeft))
    {
        Pillar->Destroy();
        return nullptr;
    }
    Pillar->FinishSpawning(Pillar->GetActorTransform());
    return IsValid(Pillar) ? Pillar : nullptr;
}

bool UBossManagerSubsystem::PlaceBossPillar(ABossPillarActor* Pillar, bool bLeft)
{
    UWorld* World = GetWorld();
    UGridManagerSubsystem* GridManager = IsValid(World) ? World->GetSubsystem<UGridManagerSubsystem>() : nullptr;
    if (!IsValid(Pillar) || !IsValid(GridManager) || GridManager->GridXSize < 9 || GridManager->GridYSize < 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossPillar] Placement requires a valid pillar and a boss area of at least 9x3."));
        return false;
    }

    constexpr int32 AreaSize = 3;
    const int32 Size = Pillar->GetFootprintSize();
    const int32 CenterX = GridManager->GridXSize / 2;
    const int32 AreaStartX = bLeft ? CenterX - 4 : CenterX + 2;
    const int32 AreaStartY = GridManager->GetBossAreaStartY();
    const int32 StartX = FMath::RandRange(AreaStartX, AreaStartX + AreaSize - Size);
    const int32 StartY = FMath::RandRange(AreaStartY, AreaStartY + AreaSize - Size);

    TArray<FGridPoint> Cells;
    FVector Center = FVector::ZeroVector;
    for (int32 Y = 0; Y < Size; ++Y)
    {
        for (int32 X = 0; X < Size; ++X)
        {
            const FGridPoint Cell{StartX + X, StartY + Y};
            FVector Location;
            if (!IsValid(GridManager->GetGridActor(Cell)) || !GridManager->TryGetGridWorldLocation(Cell, Location))
                return false;
            Cells.Add(Cell);
            Center += Location;
        }
    }
    Pillar->SetGridPlacement(Cells, Center / Cells.Num());
    return true;
}

void UBossManagerSubsystem::DestroyBossPillars()
{
    if (IsValid(LeftPillar)) LeftPillar->Destroy();
    if (IsValid(RightPillar)) RightPillar->Destroy();
    LeftPillar = nullptr;
    RightPillar = nullptr;
}

void UBossManagerSubsystem::HandleBossDestroyed(AActor* DestroyedActor)
{
    if (DestroyedActor != CurrentBoss) return;
    DestroyBossPillars();
    CurrentBoss = nullptr;
}

bool UBossManagerSubsystem::StartBossSetting()
{
    if (!IsValid(CurrentBoss))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] StartBossSetting failed: CurrentBoss null"));
        return false;
    }

    CurrentBoss->BeginPillarTurn();
    if (IsValid(LeftPillar)) PlaceBossPillar(LeftPillar, true);
    if (IsValid(RightPillar)) PlaceBossPillar(RightPillar, false);

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
    ClearCurrentPhase();

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

    // 방어막이 가득 찬 경우 ShieldHeal 패턴 제외
    UBossPatternBase* PatternObj = CurrentBoss->GetPattern();
    TArray<int32> CandidateIndices;
    const bool bShieldFull = CurrentBoss->GetCurrentShield() >= CurrentBoss->GetMaxShield() && CurrentBoss->GetMaxShield() > 0;
    for (int32 i = 0; i < PatternCount; ++i)
    {
        if (PatternObj && PatternObj->PatternData.IsValidIndex(i) && bShieldFull && PatternObj->PatternData[i].ShieldHeal > 0)
        {
            continue;
        }
        CandidateIndices.Add(i);
    }
    if (CandidateIndices.IsEmpty())
    {
        CandidateIndices.Add(FMath::RandRange(0, PatternCount - 1));
    }
    const int32 PatternIndex = CandidateIndices[FMath::RandRange(0, CandidateIndices.Num() - 1)];
    UBossPatternBase* PickedPattern = CurrentBoss->GetPattern();
    if (!PickedPattern)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] PrepareCurrentPattern failed: picked pattern null"));
        return false;
    }

    PhaseContext.CurrentPattern = PickedPattern;
    PhaseContext.CurrentPatternIndex = PatternIndex;
    PhaseContext.CurrentPattern->BuildTargetCells(CurrentBoss, PhaseContext.LockedCells, PhaseContext.CurrentPatternIndex);

    if (PickedPattern->PatternData.IsValidIndex(PhaseContext.CurrentPatternIndex))
    {
        FBossPatternBattleData CurrentPatternData = PickedPattern->PatternData[PhaseContext.CurrentPatternIndex];
        CurrentBoss->SetPatternAnim(CurrentPatternData.PatternMontage);

        // 패턴의 GimmickType으로 GimmickList에서 해당 기믹 찾아 ActiveGimmick 세팅
        UBossGimmickBase* MatchedGimmick = nullptr;
        if (CurrentPatternData.GimmickType != EBossGimmickType::None)
        {
            for (UBossGimmickBase* G : CurrentBoss->GetGimmickList())
            {
                if (IsValid(G) && G->GimmickData.GimmickType == CurrentPatternData.GimmickType)
                {
                    MatchedGimmick = G;
                    break;
                }
            }
        }
        // Proxy를 통해 ActiveGimmick 세팅:
        // OnPlayerPhaseStart/End → GimmickList 전체 브로드캐스트
        // OnPatternExecute/OnDamageCalculate/OnPhaseEnd → MatchedGimmick에만 전달
        UBossGimmick_Proxy* Proxy = NewObject<UBossGimmick_Proxy>(CurrentBoss);
        Proxy->SelectedGimmick = MatchedGimmick;
        Proxy->GimmickData = MatchedGimmick ? MatchedGimmick->GimmickData : FBossGimmickData{};
        CurrentBoss->SetActiveGimmick(Proxy);


        CurrentBoss->SetCurrentPatternInfo(PhaseContext.CurrentPatternIndex, CurrentPatternData);
    }

    // 늪 설치 패턴(bNoDamage)이면 보라색, 일반 공격이면 빨간색으로 예고
    // 늪이 깔린 셀 위로 공격이 겹치면 주황색으로 구분
    const bool bIsInstallPattern = PickedPattern->PatternData.IsValidIndex(PhaseContext.CurrentPatternIndex)
        && PickedPattern->PatternData[PhaseContext.CurrentPatternIndex].bNoDamage;
    const FLinearColor NormalTelegraphColor = bIsInstallPattern
        ? FLinearColor(0.5f, 0.1f, 0.9f, 1.f)
        : FLinearColor(1.f, 0.f, 0.f, 1.f);
    ShowTelegraphPreviewWithSwamp(PhaseContext.LockedCells, NormalTelegraphColor);

    PhaseContext.bPrepared = true;
    return true;
}


TSoftClassPtr<ABase_PatternVisualActor> UBossManagerSubsystem::GetCurrentPatternVisualClass() const
{
    if (PhaseContext.CurrentPattern && PhaseContext.CurrentPattern->PatternData.IsValidIndex(PhaseContext.CurrentPatternIndex))
    {
        return PhaseContext.CurrentPattern->PatternData[PhaseContext.CurrentPatternIndex].VisualActorClass;
    }
    
    return nullptr;
}

void UBossManagerSubsystem::ExecuteCurrentPattern()
{
    if (bAttackExecuting) return;
    if (!PhaseContext.bPrepared)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] ExecuteCurrentPattern skipped: not prepared"));
        if (IsValid(CurrentBoss))
            CurrentBoss->FinishBossAttack();
        return;
    }

    if (!IsValid(CurrentBoss))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossManager] ExecuteCurrentPattern skipped: CurrentBoss null"));
        ClearCurrentPhase();
        return;
    }

    if (!CurrentBoss->ConsumeCCForBossPhase())
    {
        ClearCurrentPhase();
        CurrentBoss->FinishBossAttack();
        return;
    }

    BuildLockedTargetsFromCells(PhaseContext.LockedCells, PhaseContext.LockedTargets);

    bAttackExecuting = true;
    bPatternApplied = false;
    bVisualActStarted = false;
    CurrentBoss->PlayAttack();
}

void UBossManagerSubsystem::ApplyCurrentPattern()
{
    if (!bAttackExecuting || bPatternApplied || !PhaseContext.bPrepared) return;

    // 지연 피해 판정 사이에 기절해도 패턴/기믹 실행을 멈추고 완료를 통지합니다.
    if (!IsValid(CurrentBoss)) { ClearCurrentPhase(); return; }
    if (CurrentBoss->IsStunned())
    {
        ClearCurrentPhase();
        CurrentBoss->FinishBossAttack();
        return;
    }

    bPatternApplied = true;
    TArray<ACoinActor*> ValidLockedTargets;
    TArray<ABase_OtherActor*> ValidLockedOthers;
    for (const FLockedBossTarget& LockedTarget : PhaseContext.LockedTargets)
    {
        if (IsValid(LockedTarget.CoinActor) && IsStillOnLockedCell(LockedTarget))
        {
            ValidLockedTargets.Add(LockedTarget.CoinActor);
        }
        else if (IsValid(LockedTarget.OtherActor))
        {
            ValidLockedOthers.Add(LockedTarget.OtherActor);
        }
    }

    if (IsValid(CurrentBoss))
    {
        PhaseContext.BaseDamage = CurrentBoss->GetAttackPoint();
        PhaseContext.BonusDamage = 0;
        PhaseContext.DamageMultiplier = 1.f;
        PhaseContext.bSkipAttack = false;
    }

    if (IsValid(CurrentBoss) && PhaseContext.CurrentPattern)
    {
        for (UBossGimmickBase* G : CurrentBoss->GetGimmickList())
        {
            G->OnBeforePatternExecute(CurrentBoss, PhaseContext);
        }
    }

    if (PhaseContext.CurrentPattern)
    {
        PhaseContext.CurrentPattern->ExecutePattern(
            CurrentBoss,
            PhaseContext,
            ValidLockedTargets,
            ValidLockedOthers
        );
    }

    // VFX 노티파이를 위해 공격 몽타주 종료까지 패턴과 대상 칸을 유지합니다.
}

void UBossManagerSubsystem::PlayCurrentVisualAct()
{
    if (!bAttackExecuting || bVisualActStarted || !IsValid(CurrentBoss) || CurrentBoss->IsStunned()) return;
    UWorld* World = GetWorld();
    UBattleLevelActingWSubsystem* Acting = IsValid(World) ? World->GetSubsystem<UBattleLevelActingWSubsystem>() : nullptr;
    if (!IsValid(Acting)) return;
    bVisualActStarted = true;
    Acting->PlayBossPatternAct();
}

void UBossManagerSubsystem::FinishCurrentAttack(bool bInterrupted)
{
    if (!bAttackExecuting) return;
    if (!bInterrupted && !bPatternApplied)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BossNotify] Attack ended without BossApplyPattern. Boss=%s PatternIndex=%d"),
            *GetNameSafe(CurrentBoss), PhaseContext.CurrentPatternIndex);
    }
    ClearCurrentPhase();
}

void UBossManagerSubsystem::ClearCurrentPhase()
{
    bAttackExecuting = false;
    bPatternApplied = false;
    bVisualActStarted = false;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TelegraphTimerHandle);
    }

    if (PhaseContext.LockedCells.Num() > 0)
    {
        ClearTelegraphPreview(PhaseContext.LockedCells);
    }

    if (IsValid(CurrentBoss))
    {
        for (UBossGimmickBase* G : CurrentBoss->GetGimmickList())
        {
            if (IsValid(G)) G->OnPhaseEnd(CurrentBoss);
        }
    }

    PhaseContext.Reset();
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
            Grid->ApplyCellMaterialParams(Color, 0.3f, 0.0f);
        }
    }
}

void UBossManagerSubsystem::ShowTelegraphPreviewWithSwamp(const TArray<FGridPoint>& Cells, const FLinearColor& Color)
{
    UGridManagerSubsystem* GridMgr = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if (!GridMgr) return;

    for (const FGridPoint& Cell : Cells)
    {
        if (AGridActor* Grid = GridMgr->GetGridActor(Cell))
        {
            Grid->bIsBossAttack = true;
            Grid->ApplyCellMaterialParams(Color, 0.9f, 0.0f);

            if (Grid->HasSwamp())
            {
                Grid->InitColor();
            }
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
            Grid->ClearBossAttackFlag();
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
        if (!IsValid(Info.CoinActor)) continue;
        if (!IsCellIncluded(Info.GridXY, Cells)) continue;

        FLockedBossTarget NewTarget;
        NewTarget.CoinID = Info.CoinID;
        NewTarget.LockedGrid = Info.GridXY;
        NewTarget.CoinActor = Info.CoinActor;
        OutLockedTargets.Add(NewTarget);
    }

    for (const FGridPoint& Cell : Cells)
    {
        AGridActor* GridActor = GridMgr->GetGridActor(Cell);
        if (!IsValid(GridActor)) continue;

        const EGridOccupyingType Type = GridActor->GetCurrentOccupyingThing();
        if (Type != EGridOccupyingType::Wall && Type != EGridOccupyingType::Turret) continue;

        ABase_OtherActor* Other = Cast<ABase_OtherActor>(GridActor->GetCurrentOccupied());
        if (!IsValid(Other)) continue;

        FLockedBossTarget NewTarget;
        NewTarget.LockedGrid = Cell;
        NewTarget.OtherActor = Other;
        OutLockedTargets.Add(NewTarget);
    }
}

void UBossManagerSubsystem::BroadcastCoinLanded()
{
    if (!IsValid(CurrentBoss)) return;
    for (UBossGimmickBase* G : CurrentBoss->GetGimmickList())
    {
        if (IsValid(G))
            G->OnCoinLanded(CurrentBoss, PhaseContext);
    }
}

void UBossManagerSubsystem::RecalculateTelegraphForRoleTarget()
{
    if (!PhaseContext.CurrentPattern) return;
    ClearTelegraphPreview(PhaseContext.LockedCells);
    PhaseContext.LockedCells.Reset();
    PhaseContext.CurrentPattern->BuildTargetCells(CurrentBoss, PhaseContext.LockedCells, PhaseContext.CurrentPatternIndex);
    ShowTelegraphPreview(PhaseContext.LockedCells, FLinearColor(1.f, 0.f, 0.f, 1.f));
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

