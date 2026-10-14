#include "BossPatternPreviewActor.h"
#include "GridActor.h"
#include "GridManagerSubsystem.h"
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

void ABossPatternPreviewActor::ShowPatternPreview(const FAttackAreaSpec& Spec, bool bNoDamage)
{
	ClearPreview();

	const UGridManagerSubsystem* BattleGrid = GetDefault<UGridManagerSubsystem>();
	if (!IsValid(BattleGrid) || GridXSize <= 0 || GridYSize <= 0) return;

	FAttackAreaSpec PreviewSpec = Spec;
	if (PreviewSpec.AnchorMode == EAreaAnchor::UseIndex)
	{
		if (PreviewSpec.Pattern == EAttackAreaPattern::Row)
			PreviewSpec.Index = GridYSize / 2;
		else if (PreviewSpec.Pattern == EAttackAreaPattern::Column)
			PreviewSpec.Index = GridXSize / 2;
		else if (PreviewSpec.Pattern == EAttackAreaPattern::ConeFromSide)
		{
			// 전투의 원뿔 강타와 같은 중앙 위쪽 시작점/아래 방향입니다.
			PreviewSpec.Index = GridXSize / 2;
			PreviewSpec.Side = EAreaSide::Down;
		}
	}
	else if (PreviewSpec.Pattern == EAttackAreaPattern::CircleOnCell ||
		PreviewSpec.Pattern == EAttackAreaPattern::CrossOnCell)
	{
		// 전투에서 랜덤으로 고르는 중심의 대표 위치입니다. DB의 (0,0) 때문에 범위가 잘리지 않게 합니다.
		PreviewSpec.AnchorCell = FGridPoint(GridXSize / 2, GridYSize / 2);
	}
	else if (PreviewSpec.Pattern == EAttackAreaPattern::RectFromCell)
	{
		// 전투와 같은 앵커 제한을 적용해 사각형 전체가 플레이 영역 안에 들어오도록 합니다.
		const int32 HalfW = FMath::Max(1, PreviewSpec.ParamA) / 2;
		const int32 Depth = FMath::Max(1, PreviewSpec.ParamB);
		const int32 MinX = FMath::Min(HalfW, (GridXSize - 1) / 2);
		const int32 MaxX = FMath::Max(MinX, GridXSize - 1 - HalfW);
		int32 MinY = 0;
		int32 MaxY = GridYSize - 1;
		if (PreviewSpec.Side == EAreaSide::Up)
			MaxY = FMath::Max(0, GridYSize - 1 - Depth);
		else if (PreviewSpec.Side == EAreaSide::Down)
			MinY = FMath::Min(Depth, MaxY);
		PreviewSpec.AnchorCell.GridX = FMath::Clamp(PreviewSpec.AnchorCell.GridX, MinX, MaxX);
		PreviewSpec.AnchorCell.GridY = FMath::Clamp(PreviewSpec.AnchorCell.GridY, MinY, MaxY);
	}

	TArray<FGridPoint> Cells;
	// 전투용 서브시스템을 상점에 생성하지 않고 기본 객체의 동일한 셀 계산 함수를 사용합니다.
	BattleGrid->BuildBossAttackCells(PreviewSpec, Cells);
	const FLinearColor HighlightColor = bNoDamage ? FLinearColor(0.5f, 0.1f, 0.9f, 1.f) : AttackHighlightColor;

	for (const FGridPoint& P : Cells)
	{
		if (!IsInGrid(P.GridX, P.GridY)) continue;
		if (AGridActor* Cell = GetGridActorAt(P.GridX, P.GridY); IsValid(Cell))
		{
			Cell->bIsBossAttack = true;
			Cell->ApplyCellMaterialParams(HighlightColor, HighlightFillIntensity, 0.f);
			Cell->InitColor();
		}
	}

	HighlightedCells = Cells;
	OnPatternWanted.Broadcast();
}

void ABossPatternPreviewActor::ClearPreview()
{
	for (const FGridPoint& P : HighlightedCells)
	{
		if (AGridActor* Cell = GetGridActorAt(P.GridX, P.GridY); IsValid(Cell))
		{
			Cell->ClearBossAttackFlag();
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
	const UGridManagerSubsystem* BattleGrid = GetDefault<UGridManagerSubsystem>();
	if (!IsValid(World) || !IsValid(BattleGrid)) return;

	GridXSize = BattleGrid->GridXSize;
	GridYSize = BattleGrid->GetBossAreaStartY();

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
			if (!IsValid(NewGrid)) continue;

			NewGrid->SetGridXY(X, Y);
			NewGrid->FinishSpawning(SpawnTM);
			NewGrid->SetActorEnableCollision(false);
			NewGrid->InitColor();
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
