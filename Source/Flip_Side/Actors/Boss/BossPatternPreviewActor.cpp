#include "BossPatternPreviewActor.h"
#include "GridActor.h"
#include "GridAreaBuilder.h"
#include "Engine/World.h"
#include "Subsystem/ShopLevel/ShopBossWSubsystem.h"

ABossPatternPreviewActor::ABossPatternPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABossPatternPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	SpawnGrid();

	if (UWorld* World = GetWorld())
	{
		if (UShopBossWSubsystem* Sub = World->GetSubsystem<UShopBossWSubsystem>())
		{
			Sub->RegisterPatternPreviewActor(this);
		}
	}
}

void ABossPatternPreviewActor::ShowPatternPreview(const FAttackAreaSpec& Spec)
{
	ClearPreview();

	TArray<FGridPoint> Cells;
	FGridAreaBuilder::BuildCells(Spec, GridXSize, GridYSize, Cells);

	for (const FGridPoint& P : Cells)
	{
		if (!IsInGrid(P.GridX, P.GridY)) continue;
		if (AGridActor* Cell = GetGridActorAt(P.GridX, P.GridY))
		{
			Cell->ApplyCellMaterialParams(AttackHighlightColor, HighlightFillIntensity, 0.f);
		}
	}

	HighlightedCells = Cells;
	OnPatternWanted.Broadcast();
}

void ABossPatternPreviewActor::ClearPreview()
{
	for (const FGridPoint& P : HighlightedCells)
	{
		if (AGridActor* Cell = GetGridActorAt(P.GridX, P.GridY))
		{
			Cell->InitColor();
		}
	}
	HighlightedCells.Reset();
	OnPatternWanted.Broadcast();
}

void ABossPatternPreviewActor::RebuildGrid()
{
	DestroyGrid();
	SpawnGrid();
}

void ABossPatternPreviewActor::SpawnGrid()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!GridActorClass || !GridActorClass->IsChildOf(AGridActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("BossPatternPreviewActor: GridActorClass가 설정되지 않았습니다."));
		return;
	}

	for (int32 Y = 0; Y < GridYSize; ++Y)
	{
		for (int32 X = 0; X < GridXSize; ++X)
		{
			const FVector SpawnLoc = GridOrigin + FVector(Y * SpacingY, X * SpacingX, 0.f);
			const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);

			AGridActor* NewGrid = World->SpawnActorDeferred<AGridActor>(
				GridActorClass, SpawnTM, this, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
			if (!NewGrid) continue;

			NewGrid->SetGridXY(X, Y);
			NewGrid->FinishSpawning(SpawnTM);
			GridActors.Add(FGridPoint{ X, Y }, NewGrid);
		}
	}
}

void ABossPatternPreviewActor::DestroyGrid()
{
	ClearPreview();
	for (auto& Pair : GridActors) { if (IsValid(Pair.Value)) Pair.Value->Destroy(); }
	GridActors.Reset();
}

AGridActor* ABossPatternPreviewActor::GetGridActorAt(int32 X, int32 Y) const
{
	const FGridPoint P{ X, Y };
	if (const TObjectPtr<AGridActor>* Found = GridActors.Find(P))
		return Found->Get();
	return nullptr;
}

bool ABossPatternPreviewActor::IsInGrid(int32 X, int32 Y) const
{
	return (0 <= X && X < GridXSize) && (0 <= Y && Y < GridYSize);
}
