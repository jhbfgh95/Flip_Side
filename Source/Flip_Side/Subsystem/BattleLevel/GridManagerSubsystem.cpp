#include "GridManagerSubsystem.h"
#include "Engine/World.h"
#include "GridActor.h"
#include "GridAreaBuilder.h"
#include "CoinActor.h"
#include "BossManagerSubsystem.h"
#include "FlipSideDevloperSettings.h"
#include "WeaponRangePreviewActor.h"

bool UGridManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	UWorld* World = Cast<UWorld>(Outer);
	if(World)
    {
        FString MapName = World->GetName();
        if(MapName.Contains(TEXT("L_Stage")))
        {
            return true;
        }
    }

    return false;
}

void UGridManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!InWorld.IsGameWorld())
		return;

	InitGrid(8, 5);
	InstanceGrid();
}

void UGridManagerSubsystem::InitGrid(int32 InGridXSize, int32 InGridYSize)
{
	GridXSize = FMath::Max(0, InGridXSize);
	GridYSize = FMath::Max(0, InGridYSize);
	ClearGrid();
}

void UGridManagerSubsystem::ClearGrid()
{
	for (auto& It : GridActors)
	{
		if (IsValid(It.Value))
			It.Value->Destroy();
	}
	GridActors.Reset();
}

void UGridManagerSubsystem::InstanceGrid()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings) return;

	// DeveloperSettings���� BP_Grid SoftClass �ε�
	UClass* BPGridClass = Settings->GridActor.LoadSynchronous();
	if (!BPGridClass || !BPGridClass->IsChildOf(AGridActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("GridManager: GridActor class invalid (must be BP child of AGridActor)."));
		return;
	}

	for (int32 Y = 0; Y < GridYSize; ++Y)       // ����
	{
		for (int32 X = 0; X < GridXSize; ++X)   // ����
		{
			const FVector SpawnLoc = GridOrigin + FVector(Y * SpacingY, X * SpacingX, 0.f);
			const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);

			AGridActor* NewGrid = World->SpawnActorDeferred<AGridActor>(
				BPGridClass, SpawnTM, nullptr, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

			if (!NewGrid) continue;

            NewGrid->OnGridClicked.AddDynamic(this, &UGridManagerSubsystem::BindForGridClick);

			NewGrid->SetGridXY(X, Y);     
			NewGrid->FinishSpawning(SpawnTM);

			FGridPoint P;
			P.GridX = X;
			P.GridY = Y;

			GridActors.Add(P, NewGrid);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GridManager: Spawned grids %dx%d"), GridXSize, GridYSize);
}


AGridActor* UGridManagerSubsystem::GetGridActor(const FGridPoint& P) const
{
    if (const TObjectPtr<AGridActor>* Found = GridActors.Find(P))
	{
		return Found->Get();
	}
	return nullptr;
}

AGridActor* UGridManagerSubsystem::GetGridActorAt(int32 X, int32 Y) const
{
    const FGridPoint P{ X, Y };
    if (const TObjectPtr<AGridActor>* Found = GridActors.Find(P))
    {
        return Found->Get();
    }
    return nullptr;
}

void UGridManagerSubsystem::CollectOccupiedCoins(TArray<FCoinOnGridInfo>& OutCoins) const
{
	OutCoins.Reset();

	for (const auto& Pair : GridActors)
	{
		const FGridPoint& GridP = Pair.Key;
		AGridActor* Grid = Pair.Value.Get();
		if (!IsValid(Grid)) continue;

		if (Grid->GetCurrentOccupyingThing() != EGridOccupyingType::Coin)
			continue;

		ACoinActor* Coin = Cast<ACoinActor>(Grid->GetCurrentOccupied());
		if (!IsValid(Coin))
			continue;

		FCoinOnGridInfo Info;
		Info.CoinID = Coin->GetCoinID();
		Info.GridXY = GridP; 
        Info.CoinActor = Coin;

		OutCoins.Add(Info);
	}
}

//park
void UGridManagerSubsystem::InitCoinOccupied()
{
    for (const auto& Pair : GridActors)
	{
		const FGridPoint& GridP = Pair.Key;
		AGridActor* Grid = Pair.Value.Get();
		if (!IsValid(Grid)) continue;

		if (Grid->GetCurrentOccupyingThing() != EGridOccupyingType::Coin)
			continue;

		Grid->ClearOccupied();
	}
}

//park
void UGridManagerSubsystem::GetObjectsAtRange(
    const FAttackAreaSpec& Spec,
    const FGridPoint& FinalRange,
    TArray<FGridPoint>& OutCells,
    FObjectOnGridInfo& Infos) const
{
    OutCells.Reset();
    Infos.Coins.Reset();
    Infos.Others.Reset();
    Infos.Boss = nullptr;

    // ---------------------------------
    // 1) 보스 라인 도달 여부를 먼저 계산
    //    (BuildCells는 보드 밖 셀을 잘라버리므로 별도 계산 필요)
    // ---------------------------------
    bool bReachedBossLine = false;

    const int32 AnchorX = FinalRange.GridX + Spec.AnchorCell.GridX;
    const int32 AnchorY = FinalRange.GridY + Spec.AnchorCell.GridY;

    switch (Spec.Pattern)
    {
    case EAttackAreaPattern::RectFromCell:
    {
        const int32 Depth = FMath::Max(1, Spec.ParamB);

        if (Spec.Side == EAreaSide::Up)
        {
            const int32 MaxY = AnchorY + Depth;
            bReachedBossLine = (MaxY >= GridYSize);
        }
        break;
    }

    case EAttackAreaPattern::CrossOnCell:
    {
        const int32 HalfY = FMath::Max(0, Spec.ParamB);
        const int32 MaxY = AnchorY + HalfY;
        bReachedBossLine = (MaxY >= GridYSize);
        break;
    }

    case EAttackAreaPattern::CircleOnCell:
    {
        const int32 Radius = FMath::Max(0, Spec.ParamA);
        const int32 MaxY = AnchorY + Radius;
        bReachedBossLine = (MaxY >= GridYSize);
        break;
    }

    case EAttackAreaPattern::SingleCell:
    {
        // SingleCell은 후보 셀을 하나 지정하는 개념이므로
        // 기본적으로 보스 라인 판정은 하지 않음.
        // 필요하면 나중에 별도 규칙 추가 가능.
        bReachedBossLine = false;
        break;
    }

    default:
        break;
    }

    // ---------------------------------
    // 2) 실제 보드 안 셀들 생성
    // ---------------------------------
    if (Spec.Pattern == EAttackAreaPattern::SingleCell)
    {
        // SingleCell은 범위 공격이 아니라 "선택 가능한 셀" 개념
        const_cast<UGridManagerSubsystem*>(this)->GetValidGridsForSingleCell(
            FinalRange,
            Spec,
            OutCells
        );
    }
    else
    {
        FAttackAreaSpec ResolvedSpec = Spec;

        // UseAnchorCell 계열은 FinalRange(현재 코인/시전자 위치)를 기준으로
        // 상대 오프셋 AnchorCell을 실제 절대 좌표로 바꿔준다.
        if (ResolvedSpec.AnchorMode == EAreaAnchor::UseAnchorCell)
        {
            ResolvedSpec.AnchorCell.GridX = FinalRange.GridX + Spec.AnchorCell.GridX;
            ResolvedSpec.AnchorCell.GridY = FinalRange.GridY + Spec.AnchorCell.GridY;
        }

        FGridAreaBuilder::BuildCells(ResolvedSpec, GridXSize, GridYSize, OutCells);
    }

    // ---------------------------------
    // 3) 보드 안 오브젝트 수집
    // ---------------------------------
    for (const FGridPoint& Grid : OutCells)
    {
        if (Grid.GridX < 0 || Grid.GridX >= GridXSize ||
            Grid.GridY < 0 || Grid.GridY >= GridYSize)
        {
            continue;
        }

        AGridActor* TargetGrid = GetGridActor(Grid);
        if (!IsValid(TargetGrid))
        {
            continue;
        }

        if (!TargetGrid->GetIsOccupied())
        {
            continue;
        }

        switch (TargetGrid->GetCurrentOccupyingThing())
        {
        case EGridOccupyingType::Coin:
        {
            Infos.Coins.Add(TargetGrid->GetCurrentOccupied());
            break;
        }

        case EGridOccupyingType::Wall:
        {
            Infos.Others.Add(TargetGrid->GetCurrentOccupied());
            break;
        }

        default:
            break;
        }
    }

    // ---------------------------------
    // 4) 보스 라인에 닿았으면 보스 연결
    // ---------------------------------
    if (bReachedBossLine)
    {
        if (UWorld* World = GetWorld())
        {
            if (UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>())
            {
                ABossActor* Boss = BossMgr->GetCurrentBoss();
                if (IsValid(Boss))
                {
                    Infos.Boss = Boss;
                }
            }
        }
    }
}

void UGridManagerSubsystem::GetValidGridsForSingleCell(
    const FGridPoint& CoinXY,
    const FAttackAreaSpec& Spec,
    TArray<FGridPoint>& ValidCells)
{
    ValidCells.Reset();

    if (Spec.Pattern != EAttackAreaPattern::SingleCell)
    {
        return;
    }

    auto AddUniqueCell = [&](int32 X, int32 Y)
        {
            if (!IsInGrid(X, Y))
            {
                return;
            }

            const FGridPoint P{ X, Y };

            for (const FGridPoint& Existing : ValidCells)
            {
                if (Existing.GridX == P.GridX && Existing.GridY == P.GridY)
                {
                    return;
                }
            }

            ValidCells.Add(P);
        };

    auto RemoveCasterCellIfNeeded = [&]()
        {
            // Flags == 1 이면 현재 코인 위치(CoinXY) 제외
            if (Spec.Flags == 1)
            {
                ValidCells.RemoveAll([&](const FGridPoint& Cell)
                    {
                        return Cell.GridX == CoinXY.GridX && Cell.GridY == CoinXY.GridY;
                    });
            }
        };

    const int32 AnchorX = CoinXY.GridX + Spec.AnchorCell.GridX;
    const int32 AnchorY = CoinXY.GridY + Spec.AnchorCell.GridY;

    // 1) UseAnchorCell
    if (Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
    {
        const int32 Radius = FMath::Max(0, Spec.ParamA);

        for (int32 dy = -Radius; dy <= Radius; ++dy)
        {
            for (int32 dx = -Radius; dx <= Radius; ++dx)
            {
                AddUniqueCell(AnchorX + dx, AnchorY + dy);
            }
        }

        RemoveCasterCellIfNeeded();
        return;
    }

    // 2) UseIndex (public vector)
    const int32 Width = FMath::Max(1, Spec.ParamA);
    const int32 Depth = FMath::Max(1, Spec.ParamB);

    const int32 HalfL = (Width - 1) / 2;
    const int32 HalfR = Width / 2;

    switch (Spec.Side)
    {
    case EAreaSide::Up:
        for (int32 d = 1; d <= Depth; ++d)
        {
            for (int32 dx = -HalfL; dx <= HalfR; ++dx)
            {
                AddUniqueCell(AnchorX + dx, AnchorY + d);
            }
        }
        break;

    case EAreaSide::Down:
        for (int32 d = 1; d <= Depth; ++d)
        {
            for (int32 dx = -HalfL; dx <= HalfR; ++dx)
            {
                AddUniqueCell(AnchorX + dx, AnchorY - d);
            }
        }
        break;

    case EAreaSide::Left:
        for (int32 d = 1; d <= Depth; ++d)
        {
            for (int32 dy = -HalfL; dy <= HalfR; ++dy)
            {
                AddUniqueCell(AnchorX - d, AnchorY + dy);
            }
        }
        break;

    case EAreaSide::Right:
        for (int32 d = 1; d <= Depth; ++d)
        {
            for (int32 dy = -HalfL; dy <= HalfR; ++dy)
            {
                AddUniqueCell(AnchorX + d, AnchorY + dy);
            }
        }
        break;

    default:
        break;
    }

    RemoveCasterCellIfNeeded();
}

// ======================
// Boss Attack Cell Build
// ======================

void UGridManagerSubsystem::BuildBossAttackCells(const FAttackAreaSpec& Spec, TArray<FGridPoint>& OutCells) const
{
    OutCells.Reset();

	FGridAreaBuilder::BuildCells(Spec, GridXSize, GridYSize, OutCells);
}

// ======================
// Lambda-based Single Target Spec
// ======================

void UGridManagerSubsystem::BuildCoinTargetCells(
    FCoinPred Predicate,
    TArray<FGridPoint>& OutCells
) const
{
    OutCells.Reset();

    TArray<FCoinOnGridInfo> Coins;
    CollectOccupiedCoins(Coins);

    for (const FCoinOnGridInfo& C : Coins)
    {
        if (Predicate(C))
        {
            OutCells.Add(C.GridXY); 
        }
    }
}

void UGridManagerSubsystem::PreviewHoveredCoinRange(const FGridPoint& CoinXY, const FAttackAreaSpec& Spec, const FGridPoint& finalRange)
{
	UE_LOG(LogTemp, Log, TEXT("PreviewHoveredCoinRange 호출됨 - CoinXY(%d,%d), PreviewActor유효:%s"),
		CoinXY.GridX, CoinXY.GridY, IsValid(PreviewActor) ? TEXT("YES") : TEXT("NO"));

	ResetBattleCoinPreview();

	// 사거리 계산 (단일 진실 공급원)
	TArray<FGridPoint> RangeCells;

	if (Spec.Pattern == EAttackAreaPattern::SingleCell)
	{
		GetValidGridsForSingleCell(CoinXY, Spec, RangeCells);
	}
	else
	{
		FGridAreaBuilder::BuildCells(Spec, GridXSize, GridYSize, RangeCells);
	}

	// 메인 그리드 하이라이트
	if (AGridActor* CoinCell = GetGridActorAt(CoinXY.GridX, CoinXY.GridY))
	{
        CoinCell->bIsCoinRangePreview = true;
		CoinCell->CoinRangeSet.Color = FLinearColor(0.f, 1.f, 0.3f, 1.f);
		CoinCell->CoinRangeSet.Intensity = 0.8f;
		CoinCell->CoinRangeSet.DoorOpen = 0.f;
		CoinCell->InitColor();
	}
	MainPreviewCoinCell = CoinXY;

	for (const FGridPoint& P : RangeCells)
	{
		if (P.GridX == CoinXY.GridX && P.GridY == CoinXY.GridY) continue;

		if (AGridActor* Cell = GetGridActorAt(P.GridX, P.GridY))
		{
			Cell->bIsCoinRangePreview = true;
			Cell->CoinRangeSet.Color = FLinearColor(1.f, 0.5f, 0.f, 1.f);
			Cell->CoinRangeSet.Intensity = 0.8f;
			Cell->CoinRangeSet.DoorOpen = 0.f;
			Cell->InitColor();
		}
	}
	MainPreviewHighlightedCells = RangeCells;

	// PreviewActor에 계산된 RangeCells 전달
	if (IsValid(PreviewActor))
	{
		PreviewActor->ShowPreview(CoinXY, RangeCells);
	}
}

void UGridManagerSubsystem::ResetBattleCoinPreview()
{
	if (IsValid(PreviewActor))
	{
		PreviewActor->ClearPreview();
	}

	// 메인 그리드 초기화
	for (const FGridPoint& P : MainPreviewHighlightedCells)
	{
		if (AGridActor* Cell = GetGridActorAt(P.GridX, P.GridY))
		{
            Cell->bIsCoinRangePreview = false;
			Cell->InitColor();
		}
	}
	MainPreviewHighlightedCells.Reset();

	if (AGridActor* CoinCell = GetGridActorAt(MainPreviewCoinCell.GridX, MainPreviewCoinCell.GridY))
	{
        CoinCell->bIsCoinRangePreview = false;
		CoinCell->InitColor();
	}
	MainPreviewCoinCell = FGridPoint{ -1, -1 };
}

bool UGridManagerSubsystem::IsInGrid(int32 X, int32 Y) const
{
    return (0 <= X && X < GridXSize) && (0 <= Y && Y < GridYSize);
}


void UGridManagerSubsystem::BindForGridClick(AGridActor* targetGrid)
{
    if(!targetGrid || ClickFlag == EGridClickFlag::None) return;

    if(ClickFlag == EGridClickFlag::CoinAction)
    {
        OnGridClickedForCoin.ExecuteIfBound(targetGrid);
    }
    else if(ClickFlag == EGridClickFlag::ItemAction)
    {
        OnGridClickedForItem.ExecuteIfBound(targetGrid);
    }
}

void UGridManagerSubsystem::StopDoorFx(const FGridPoint& Cell)
{
    if (!GetWorld()) return;

    if (FCellDoorFxState* State = DoorFxByCell.Find(Cell))
    {
        GetWorld()->GetTimerManager().ClearTimer(State->Phase1Tick);
        GetWorld()->GetTimerManager().ClearTimer(State->Phase2Tick);
        DoorFxByCell.Remove(Cell);
    }
}

void UGridManagerSubsystem::PlaySingleCellDoorOpenFx(int32 GridX, int32 GridY, float PhaseDuration)
{
    if (!GetWorld()) return;
    if (!IsInGrid(GridX, GridY)) return;

    AGridActor* CellActor = GetGridActorAt(GridX, GridY);
    if (!CellActor) return;

    const FGridPoint Cell{ GridX, GridY };

    // ���� ĭ ���� ���̸� ���� �� �����
    StopDoorFx(Cell);

    FCellDoorFxState State;
    State.PhaseDuration = FMath::Max(0.01f, PhaseDuration);
    State.Phase1StartTime = GetWorld()->GetTimeSeconds();
    DoorFxByCell.Add(Cell, State);

    CellActor->ApplyCellMaterialParams(
        FLinearColor(0.f, 0.f, 0.f, 1.f),  // Outline 000000FF
        0.8f,                               // Fill_intensity
        0.0f                                // Door_Open
    );

    // Phase1 Tick ����
    GetWorld()->GetTimerManager().SetTimer(
        DoorFxByCell[Cell].Phase1Tick,
        FTimerDelegate::CreateUObject(this, &UGridManagerSubsystem::TickPhase1, Cell),
        0.016f,
        true
    );
}

void UGridManagerSubsystem::TickPhase1(FGridPoint Cell)
{
    if (!GetWorld()) { StopDoorFx(Cell); return; }

    FCellDoorFxState* State = DoorFxByCell.Find(Cell);
    if (!State) return;

    AGridActor* CellActor = GetGridActorAt(Cell.GridX, Cell.GridY);
    if (!CellActor) { StopDoorFx(Cell); return; }

    const float Now = GetWorld()->GetTimeSeconds();
    const float Alpha = FMath::Clamp((Now - State->Phase1StartTime) / State->PhaseDuration, 0.f, 1.f);

    // Door_Open: 0 -> 0.4
    const float Door = FMath::Lerp(0.0f, 0.4f, Alpha);

    CellActor->ApplyCellMaterialParams(
        FLinearColor(0.f, 0.f, 0.f, 1.f),
        0.8f,
        Door
    );

    if (Alpha >= 1.f)
    {
        GetWorld()->GetTimerManager().ClearTimer(State->Phase1Tick);
        StartPhase2(Cell);
    }
}

void UGridManagerSubsystem::StartPhase2(FGridPoint Cell)
{
    if (!GetWorld()) { StopDoorFx(Cell); return; }

    FCellDoorFxState* State = DoorFxByCell.Find(Cell);
    if (!State) return;

    AGridActor* CellActor = GetGridActorAt(Cell.GridX, Cell.GridY);
    if (!CellActor) { StopDoorFx(Cell); return; }

    // Phase2 ���� �ð� ���
    State->Phase2StartTime = GetWorld()->GetTimeSeconds();

    // Phase2 Tick ���� (Door_Open: 0.4 -> 0 + ��/���� ����)
    GetWorld()->GetTimerManager().SetTimer(
        State->Phase2Tick,
        FTimerDelegate::CreateUObject(this, &UGridManagerSubsystem::TickPhase2, Cell),
        0.016f,
        true
    );
}

void UGridManagerSubsystem::TickPhase2(FGridPoint Cell)
{
    if (!GetWorld()) { StopDoorFx(Cell); return; }

    FCellDoorFxState* State = DoorFxByCell.Find(Cell);
    if (!State) return;

    AGridActor* CellActor = GetGridActorAt(Cell.GridX, Cell.GridY);
    if (!CellActor) { StopDoorFx(Cell); return; }

    const float Now = GetWorld()->GetTimeSeconds();
    const float Alpha = FMath::Clamp((Now - State->Phase2StartTime) / State->PhaseDuration, 0.f, 1.f);

    // Door_Open: 0.4 -> 0
    const float Door = FMath::Lerp(0.4f, 0.0f, Alpha);

    if(!CellActor->bIsBossAttack)
    {
        CellActor->ApplyCellMaterialParams(
            FLinearColor(1.f, 1.f, 1.f, 1.f),  // Outline FFFFFFFF
            0.03f,                               // Fill_intensity ����
            Door
        );
    }
    else
    {
        CellActor->InitColor();
    }


    if (Alpha >= 1.f)
    {
        StopDoorFx(Cell);
    }
}


void UGridManagerSubsystem::SetGridClickFlag(EGridClickFlag clickFlag)
{ 
    ClickFlag = clickFlag;
    if(ClickFlag == EGridClickFlag::CoinAction)
    {
        for (const auto& Pair : GridActors)
        {
            const FGridPoint& GridP = Pair.Key;
            AGridActor* Grid = Pair.Value.Get();
            if (!IsValid(Grid)) continue;

            if(Grid->GetIsOccupied()) continue;
            Grid->HoverFlag = 1;

        }
    }
    else if(ClickFlag == EGridClickFlag::ItemAction)
    {
        for (const auto& Pair : GridActors)
        {
            const FGridPoint& GridP = Pair.Key;
            AGridActor* Grid = Pair.Value.Get();
            if (!IsValid(Grid)) continue;

            if(Grid->GetIsOccupied()) continue;
            Grid->HoverFlag = 2;
        }       
    }
    else
    {
        for (const auto& Pair : GridActors)
        {
            const FGridPoint& GridP = Pair.Key;
            AGridActor* Grid = Pair.Value.Get();
            if (!IsValid(Grid)) continue;

            if(Grid->GetIsOccupied()) continue;
            Grid->HoverFlag = 0;

        }
    }
}
