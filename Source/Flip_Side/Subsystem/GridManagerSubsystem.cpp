#include "GridManagerSubsystem.h"

#include "Engine/World.h"
#include "GridActor.h"
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

	// DeveloperSettings에서 BP_Grid SoftClass 로드
	UClass* BPGridClass = Settings->GridActor.LoadSynchronous();
	if (!BPGridClass || !BPGridClass->IsChildOf(AGridActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("GridManager: GridActor class invalid (must be BP child of AGridActor)."));
		return;
	}

	for (int32 Y = 0; Y < GridYSize; ++Y)       // 세로
	{
		for (int32 X = 0; X < GridXSize; ++X)   // 가로
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

static inline bool IsInGrid(const FGridPoint& P, int32 W, int32 H)
{
    return (0 <= P.GridX && P.GridX < W) && (0 <= P.GridY && P.GridY < H);
}

static inline void AddIfIn(TArray<FGridPoint>& Out, const FGridPoint& P, int32 W, int32 H)
{
    if (IsInGrid(P, W, H))
        Out.Add(P);
}

void UGridManagerSubsystem::BuildBossAttackCells(const FBossAttackSpec& Spec, TArray<FGridPoint>& OutCells) const
{
    OutCells.Reset();

    const int32 W = GridXSize; // 8
    const int32 H = GridYSize; // 5

    auto Add = [&](int32 X, int32 Y)
        {
            FGridPoint P; P.GridX = X; P.GridY = Y;
            AddIfIn(OutCells, P, W, H);
        };

    const FGridPoint T = Spec.bUseTargetCell ? Spec.TargetCell : FGridPoint{ Spec.Index, 0 };

    switch (Spec.Pattern)
    {
    case EBossAttackPattern::Column:
    {
        const int32 Col = Spec.Index;
        for (int32 Y = 0; Y < H; ++Y) Add(Col, Y);
        break;
    }
    case EBossAttackPattern::Row:
    {
        const int32 Row = Spec.Index;
        for (int32 X = 0; X < W; ++X) Add(X, Row);
        break;
    }
    case EBossAttackPattern::CrossOnCell:
    {
        Add(T.GridX, T.GridY);
        Add(T.GridX + 1, T.GridY);
        Add(T.GridX - 1, T.GridY);
        Add(T.GridX, T.GridY + 1);
        Add(T.GridX, T.GridY - 1);
        break;
    }
    case EBossAttackPattern::CircleOnCell:
    {
        const int32 R = Spec.ParamA;
        for (int32 dy = -R; dy <= R; ++dy)
            for (int32 dx = -R; dx <= R; ++dx)
                Add(T.GridX + dx, T.GridY + dy);
        break;
    }
    case EBossAttackPattern::DiagonalMain:
    {
        const int32 N = FMath::Min(W, H);
        for (int32 i = 0; i < N; ++i) Add(i, i);
        break;
    }
    case EBossAttackPattern::DiagonalAnti:
    {
        const int32 N = FMath::Min(W, H);
        for (int32 i = 0; i < N; ++i) Add((W - 1) - i, i);
        break;
    }
    case EBossAttackPattern::Border:
    {
        for (int32 X = 0; X < W; ++X) { Add(X, 0); Add(X, H - 1); }
        for (int32 Y = 1; Y < H - 1; ++Y) { Add(0, Y); Add(W - 1, Y); }
        break;
    }
    case EBossAttackPattern::ConeFromSide:
    {
        const int32 HalfWidth0 = Spec.ParamA; // 시작 폭
        const int32 Depth = Spec.ParamB;      // 깊이

        if (Spec.Side == EBossSide::Up)
        {
            const int32 CenterX = Spec.Index;
            for (int32 d = 0; d < Depth && d < H; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 Y = d;
                for (int32 X = CenterX - hw; X <= CenterX + hw; ++X) Add(X, Y);
            }
        }
        else if (Spec.Side == EBossSide::Down)
        {
            const int32 CenterX = Spec.Index;
            for (int32 d = 0; d < Depth && d < H; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 Y = (H - 1) - d;
                for (int32 X = CenterX - hw; X <= CenterX + hw; ++X) Add(X, Y);
            }
        }
        else if (Spec.Side == EBossSide::Left)
        {
            const int32 CenterY = Spec.Index;
            for (int32 d = 0; d < Depth && d < W; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 X = d;
                for (int32 Y = CenterY - hw; Y <= CenterY + hw; ++Y) Add(X, Y);
            }
        }
        else // Right
        {
            const int32 CenterY = Spec.Index;
            for (int32 d = 0; d < Depth && d < W; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 X = (W - 1) - d;
                for (int32 Y = CenterY - hw; Y <= CenterY + hw; ++Y) Add(X, Y);
            }
        }
        break;
    }
    case EBossAttackPattern::SingleTargetCoin:
    {
        if (Spec.bUseTargetCell)
            Add(Spec.TargetCell.GridX, Spec.TargetCell.GridY);
        break;
    }
    default:
        break;
    }
}

// ======================
// Boss Attack Preview (Telegraph)
// ======================

// 현재 사용 안 함
/*
void UGridManagerSubsystem::PreviewBossAttack(const FBossAttackSpec& Spec)
{
    TArray<FGridPoint> AttackCells;
    BuildBossAttackCells(Spec, AttackCells);

    for (const FGridPoint& P : AttackCells)
    {
        if (AGridActor* Grid = GetGridActor(P))
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