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

	// 너가 말한 5x8(세로5,가로8) => XSize=8, YSize=5
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
		Info.GridXY = GridP; // 또는 Grid->GetGridXY()

		OutCoins.Add(Info);
	}
}
