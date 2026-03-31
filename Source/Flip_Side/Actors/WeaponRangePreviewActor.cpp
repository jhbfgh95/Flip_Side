#include "WeaponRangePreviewActor.h"
#include "GridActor.h"
#include "GridAreaBuilder.h"
#include "Engine/World.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"

AWeaponRangePreviewActor::AWeaponRangePreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWeaponRangePreviewActor::BeginPlay()
{
	Super::BeginPlay();
	SpawnGrids();

	// GridManagerSubsystem에 자신을 등록
	if (UWorld* World = GetWorld())
	{
		UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
		if (GridMgr)
		{
			GridMgr->RegisterPreviewActor(this);
			UE_LOG(LogTemp, Log, TEXT("WeaponRangePreviewActor: GridManagerSubsystem 등록 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponRangePreviewActor: GridManagerSubsystem을 찾을 수 없음 - L_Stage 맵인지 확인하세요"));
		}
	}
}

// ──────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────

void AWeaponRangePreviewActor::ShowBothFacePreview(const FAttackAreaSpec& FrontSpec, const FAttackAreaSpec& BackSpec)
{
	ClearPreview();
	ApplyPreviewToGrid(FrontGridActors, FrontSpec, FrontHighlightedCells);
	ApplyPreviewToGrid(BackGridActors, BackSpec, BackHighlightedCells);
}

void AWeaponRangePreviewActor::ShowPreview(const FGridPoint& CoinXY, const TArray<FGridPoint>& RangeCells)
{
	// GridManagerSubsystem이 계산한 결과를 앞면 그리드에 표시 (배틀 중 호버용)
	ClearGridHighlight(FrontGridActors, FrontHighlightedCells);

	if (AGridActor* CoinAct = GetGridActorAt(FrontGridActors, CoinCell.GridX, CoinCell.GridY))
	{
		CoinAct->ApplyCellMaterialParams(CoinCellColor, RangeFillIntensity, 0.f);
	}

	for (const FGridPoint& P : RangeCells)
	{
		if (!IsInGrid(P.GridX, P.GridY)) continue;
		if (P.GridX == CoinXY.GridX && P.GridY == CoinXY.GridY) continue;

		if (AGridActor* Cell = GetGridActorAt(FrontGridActors, P.GridX, P.GridY))
		{
			Cell->ApplyCellMaterialParams(RangeHighlightColor, RangeFillIntensity, 0.f);
		}
	}
	FrontHighlightedCells = RangeCells;
}

void AWeaponRangePreviewActor::ClearPreview()
{
	ClearGridHighlight(FrontGridActors, FrontHighlightedCells);
	ClearGridHighlight(BackGridActors, BackHighlightedCells);
}

void AWeaponRangePreviewActor::RebuildGrid()
{
	DestroyGrids();
	SpawnGrids();
}

// ──────────────────────────────────────────────
// Private
// ──────────────────────────────────────────────

void AWeaponRangePreviewActor::ApplyPreviewToGrid(
	TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap,
	const FAttackAreaSpec& Spec,
	TArray<FGridPoint>& OutHighlightedCells)
{
	// CoinCell 기준으로 AnchorCell 절대 좌표 세팅
	FAttackAreaSpec ResolvedSpec = Spec;
	if (ResolvedSpec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		ResolvedSpec.AnchorCell.GridX = CoinCell.GridX + Spec.AnchorCell.GridX;
		ResolvedSpec.AnchorCell.GridY = CoinCell.GridY + Spec.AnchorCell.GridY;
	}

	TArray<FGridPoint> RangeCells;
	FGridAreaBuilder::BuildCells(ResolvedSpec, GridXSize, GridYSize, RangeCells);

	// 코인 위치 셀 표시
	if (AGridActor* CoinAct = GetGridActorAt(GridMap, CoinCell.GridX, CoinCell.GridY))
	{
		CoinAct->ApplyCellMaterialParams(CoinCellColor, RangeFillIntensity, 0.f);
	}

	// 사거리 셀 하이라이트
	for (const FGridPoint& P : RangeCells)
	{
		if (!IsInGrid(P.GridX, P.GridY)) continue;
		if (P.GridX == CoinCell.GridX && P.GridY == CoinCell.GridY) continue;

		if (AGridActor* Cell = GetGridActorAt(GridMap, P.GridX, P.GridY))
		{
			Cell->ApplyCellMaterialParams(RangeHighlightColor, RangeFillIntensity, 0.f);
		}
	}

	OutHighlightedCells = RangeCells;
}

void AWeaponRangePreviewActor::ClearGridHighlight(
	TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap,
	TArray<FGridPoint>& HighlightedCells)
{
	for (const FGridPoint& P : HighlightedCells)
	{
		if (AGridActor* Cell = GetGridActorAt(GridMap, P.GridX, P.GridY))
		{
			Cell->InitColor();
		}
	}
	HighlightedCells.Reset();

	// 코인 셀도 초기화
	if (AGridActor* CoinAct = GetGridActorAt(GridMap, CoinCell.GridX, CoinCell.GridY))
	{
		CoinAct->InitColor();
	}
}

void AWeaponRangePreviewActor::SpawnGrids()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!GridActorClass || !GridActorClass->IsChildOf(AGridActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponRangePreviewActor: GridActorClass가 설정되지 않았습니다."));
		return;
	}

	auto SpawnOneGrid = [&](const FVector& Origin, TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap)
	{
		for (int32 Y = 0; Y < GridYSize; ++Y)
		{
			for (int32 X = 0; X < GridXSize; ++X)
			{
				const FVector SpawnLoc = Origin + FVector(Y * SpacingY, X * SpacingX, 0.f);
				const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);

				AGridActor* NewGrid = World->SpawnActorDeferred<AGridActor>(
					GridActorClass, SpawnTM, this, nullptr,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
				if (!NewGrid) continue;

				NewGrid->SetGridXY(X, Y);
				NewGrid->FinishSpawning(SpawnTM);
				GridMap.Add(FGridPoint{ X, Y }, NewGrid);
			}
		}
	};

	SpawnOneGrid(FrontGridOrigin, FrontGridActors);
	SpawnOneGrid(BackGridOrigin, BackGridActors);

	UE_LOG(LogTemp, Log, TEXT("WeaponRangePreviewActor: 미리보기 그리드 2개 스폰 완료 (%dx%d)"), GridXSize, GridYSize);
}

void AWeaponRangePreviewActor::DestroyGrids()
{
	ClearPreview();

	for (auto& Pair : FrontGridActors) { if (IsValid(Pair.Value)) Pair.Value->Destroy(); }
	FrontGridActors.Reset();

	for (auto& Pair : BackGridActors) { if (IsValid(Pair.Value)) Pair.Value->Destroy(); }
	BackGridActors.Reset();
}

AGridActor* AWeaponRangePreviewActor::GetGridActorAt(TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap, int32 X, int32 Y) const
{
	const FGridPoint P{ X, Y };
	if (const TObjectPtr<AGridActor>* Found = GridMap.Find(P))
		return Found->Get();
	return nullptr;
}

bool AWeaponRangePreviewActor::IsInGrid(int32 X, int32 Y) const
{
	return (0 <= X && X < GridXSize) && (0 <= Y && Y < GridYSize);
}
