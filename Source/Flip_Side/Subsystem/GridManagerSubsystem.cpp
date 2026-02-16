#include "GridManagerSubsystem.h"
#include "Engine/World.h"
#include "GridActor.h"
#include "GridAreaBuilder.h"
#include "CoinActor.h"
#include "FlipSideDevloperSettings.h"

bool UGridManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	UWorld* World = Cast<UWorld>(Outer);
	if (!World)
		return false;

	const FString MapName = World->GetName();
	return MapName.Contains(TEXT("L_Stage"));
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

		ACoinActor* Coin = Grid->GetCurrentCoin();
		if (!IsValid(Coin))
			continue;

		FCoinOnGridInfo Info;
		Info.CoinID = Coin->GetCoinID();
		Info.GridXY = GridP; 
        Info.CoinActor = Coin;

		OutCoins.Add(Info);
	}
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
// Boss Attack Preview (Telegraph)
// ======================

// ���� ��� �� ��
/*
void UGridManagerSubsystem::PreviewBossAttack(const FAttackAreaSpec& Spec)
{
    TArray<FGridPoint> AttackCells;
    BuildBossAttackCells(Spec, AttackCells);

    for (const FGridPoint& P : AttackCells)
    {
        if (AGridActor* Grid = GetGridActorAt(P))
        {
            // Grid->SetDanger(true);
            // Grid->SetPreviewColor(FLinearColor::Red);
        }
    }
}

void UGridManagerSubsystem::ClearBossAttackPreview()
{
    for (const auto& Pair : GridActors)
    {
        if (AGridActor* Grid = Pair.Value.Get())
        {
            // Grid->SetDanger(false);
            // Grid->ResetPreviewColor();
        }
    }
}
*/

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

// GridManagerSubsystem.cpp

bool UGridManagerSubsystem::IsInGrid(int32 X, int32 Y) const
{
    return (0 <= X && X < GridXSize) && (0 <= Y && Y < GridYSize);
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

    CellActor->ApplyCellMaterialParams(
        FLinearColor(1.f, 1.f, 1.f, 1.f),  // Outline FFFFFFFF
        0.4f,                               // Fill_intensity ����
        Door
    );

    if (Alpha >= 1.f)
    {
        StopDoorFx(Cell);
    }
}
