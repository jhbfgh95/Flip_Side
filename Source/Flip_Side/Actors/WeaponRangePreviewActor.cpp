#include "WeaponRangePreviewActor.h"
#include "GridActor.h"
#include "GridAreaBuilder.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"

namespace
{
	constexpr int32 PreviewCellStarts[] = {0, 6, 51, 57};
	constexpr int32 PreviewCellCounts[] = {6, 45, 6, 45};
}

AWeaponRangePreviewActor::AWeaponRangePreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRoot"));
	FrontAttackPreviewRangeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FrontAttackPreviewRangeComponent"));
	FrontAbilityPreviewRangeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FrontAbilityPreviewRangeComponent"));
	BackAttackPreviewRangeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BackAttackPreviewRangeComponent"));
	BackAbilityPreviewRangeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BackAbilityPreviewRangeComponent"));
	FrontAttackCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FrontAttackCapture"));
	FrontAbilityCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FrontAbilityCapture"));
	BackAttackCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("BackAttackCapture"));
	BackAbilityCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("BackAbilityCapture"));
	USceneComponent* Groups[] = {FrontAttackPreviewRangeComponent, FrontAbilityPreviewRangeComponent,
		BackAttackPreviewRangeComponent, BackAbilityPreviewRangeComponent};
	USceneCaptureComponent2D* Captures[] = {FrontAttackCapture, FrontAbilityCapture, BackAttackCapture, BackAbilityCapture};
	for (int32 Group = 0; Group < 4; ++Group)
	{
		Groups[Group]->SetupAttachment(RootComponent);
		Groups[Group]->SetRelativeLocation(FVector(0.f, Group * 6000.f, 0.f));
		const int32 Width = Group % 2 == 0 ? 1 : 9;
		const int32 Height = Group % 2 == 0 ? 6 : 5;
		Captures[Group]->SetupAttachment(Groups[Group]);
		Captures[Group]->SetRelativeLocation(FVector((Height - 1) * 220.f, (Width - 1) * 220.f, 5000.f));
		Captures[Group]->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		Captures[Group]->ProjectionType = ECameraProjectionMode::Orthographic;
		Captures[Group]->OrthoWidth = 4400.f;
		Captures[Group]->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		Captures[Group]->bCaptureEveryFrame = false;
		Captures[Group]->bCaptureOnMovement = false;
		Captures[Group]->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		for (int32 Cell = 0; Cell < PreviewCellCounts[Group]; ++Cell)
		{
			const FName Name(*FString::Printf(TEXT("Range%d_Cell_%02d"), Group, Cell));
			UChildActorComponent* Child = CreateDefaultSubobject<UChildActorComponent>(Name);
			Child->SetupAttachment(Groups[Group]);
			Child->SetRelativeLocation(FVector((Cell / Width) * SpacingY, (Cell % Width) * SpacingX, 0.f));
			PreviewCells.Add(Child);
		}
	}
	AbilityCoinOrigins.Add(EAttackAreaPattern::CircleOnCell, EPreviewCoinOrigin::Center);
	AbilityCoinOrigins.Add(EAttackAreaPattern::CrossOnCell, EPreviewCoinOrigin::Center);
	AbilityCoinOrigins.Add(EAttackAreaPattern::SingleCell, EPreviewCoinOrigin::Center);
	AbilityCoinOrigins.Add(EAttackAreaPattern::RectFromCell, EPreviewCoinOrigin::Bottom);
	AbilityCoinOrigins.Add(EAttackAreaPattern::Row, EPreviewCoinOrigin::Center);
	AbilityCoinOrigins.Add(EAttackAreaPattern::Column, EPreviewCoinOrigin::Center);
}

void AWeaponRangePreviewActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ConfigureCells();
}

void AWeaponRangePreviewActor::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCells();
	if (!GridActorClass)
		UE_LOG(LogTemp, Warning, TEXT("WeaponRangePreviewActor: assign GridActorClass (BP_Grid) in the preview BP."));
	USceneCaptureComponent2D* Captures[] = {FrontAttackCapture, FrontAbilityCapture, BackAttackCapture, BackAbilityCapture};
	TSet<UTextureRenderTarget2D*> Targets;
	for (USceneCaptureComponent2D* Capture : Captures)
	{
		if (!IsValid(Capture)) continue;
		if (!IsValid(Capture->TextureTarget))
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponRangePreviewActor: assign TextureTarget on %s."), *Capture->GetName());
		}
		else if (Targets.Contains(Capture->TextureTarget))
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponRangePreviewActor: each capture needs a DIFFERENT TextureTarget (%s)."), *Capture->GetName());
		}
		else Targets.Add(Capture->TextureTarget);
	}
	ClearPreview();
}

void AWeaponRangePreviewActor::ConfigureCells()
{
	for (int32 Group = 0; Group < 4; ++Group)
	{
		const int32 Width = Group % 2 == 0 ? 1 : 9;
		for (int32 Cell = 0; Cell < PreviewCellCounts[Group]; ++Cell)
		{
			const int32 Index = PreviewCellStarts[Group] + Cell;
			if (!PreviewCells.IsValidIndex(Index) || !IsValid(PreviewCells[Index])) continue;
			UChildActorComponent* Child = PreviewCells[Index];
			Child->SetRelativeLocation(FVector((Cell / Width) * FMath::Max(1.f, SpacingY),
				(Cell % Width) * FMath::Max(1.f, SpacingX), 0.f));
			Child->SetChildActorClass(GridActorClass);
			if (AGridActor* Grid = Cast<AGridActor>(Child->GetChildActor()); IsValid(Grid))
			{
				// 전장 점유 등록 없이 기존 셀 머테리얼 기능만 재사용합니다.
				Grid->SetGridXY(Cell % Width, Cell / Width);
				Grid->SetActorEnableCollision(false);
				Grid->SetActorTickEnabled(false);
			}
		}
	}
}

void AWeaponRangePreviewActor::BuildPreviewCells(const FAttackAreaSpec& Spec, bool bAttack,
	EPreviewCoinOrigin Origin, TArray<FGridPoint>& OutCells, FGridPoint& OutCoin, bool& bOutShowCoin)
{
	OutCells.Reset();
	OutCoin = bAttack ? FGridPoint(0, 0) : FGridPoint(4, Origin == EPreviewCoinOrigin::Bottom ? 0 : 2);
	bOutShowCoin = true;
	FAttackAreaSpec Resolved = Spec;
	if (bAttack)
	{
		// 현재 DB 직선의 길이는 유지하고 설명용 방향/기준점만 정규화합니다.
		Resolved.Pattern = EAttackAreaPattern::RectFromCell;
		Resolved.AnchorMode = EAreaAnchor::UseAnchorCell;
		Resolved.AnchorCell = OutCoin;
		Resolved.Side = EAreaSide::Up;
		Resolved.ParamA = 1;
		Resolved.ParamB = FMath::Clamp(Spec.ParamB, 0, 5);
		if (Resolved.ParamB > 0) FGridAreaBuilder::BuildCells(Resolved, 1, 6, OutCells);
		return;
	}
	// 현재 DB의 전 범위는 CircleOnCell radius=8(9x9 어느 셀에서도 전체 도달)입니다.
	if (Spec.Pattern == EAttackAreaPattern::CircleOnCell && Spec.ParamA >= 8)
	{
		bOutShowCoin = false;
		for (int32 Y = 0; Y < 5; ++Y)
			for (int32 X = 0; X < 9; ++X) OutCells.Add(FGridPoint(X, Y));
		return;
	}
	// 창의 적과 같은 후방 직선 Spec은 가로로 회전한 대표 모양으로 표시합니다.
	if (Spec.Pattern == EAttackAreaPattern::RectFromCell && Spec.Side == EAreaSide::Down && Spec.ParamA == 1)
	{
		OutCoin = FGridPoint(8, 2);
		Resolved.AnchorMode = EAreaAnchor::UseAnchorCell;
		Resolved.AnchorCell = OutCoin;
		Resolved.Side = EAreaSide::Left;
		FGridAreaBuilder::BuildCells(Resolved, 9, 5, OutCells);
		return;
	}
	const bool bFixed = Spec.Pattern == EAttackAreaPattern::Border || Spec.Pattern == EAttackAreaPattern::DiagonalMain ||
		Spec.Pattern == EAttackAreaPattern::DiagonalAnti || Spec.Pattern == EAttackAreaPattern::ConeFromSide ||
		((Spec.Pattern == EAttackAreaPattern::Row || Spec.Pattern == EAttackAreaPattern::Column) && Spec.AnchorMode == EAreaAnchor::UseIndex);
	bOutShowCoin = !bFixed;
	if (bFixed)
	{
		Resolved.Index = (Spec.Pattern == EAttackAreaPattern::Row ||
			(Spec.Pattern == EAttackAreaPattern::ConeFromSide && (Spec.Side == EAreaSide::Left || Spec.Side == EAreaSide::Right))) ? 2 : 4;
	}
	else
	{
		Resolved.AnchorCell = FGridPoint(OutCoin.GridX + Spec.AnchorCell.GridX, OutCoin.GridY + Spec.AnchorCell.GridY);
		Resolved.AnchorMode = EAreaAnchor::UseAnchorCell;
		if (Spec.Pattern == EAttackAreaPattern::Row) Resolved.Index = Resolved.AnchorCell.GridY;
		if (Spec.Pattern == EAttackAreaPattern::Column) Resolved.Index = Resolved.AnchorCell.GridX;
		// SingleCell 후보 영역을 전장 점유 검사 없이 동일한 모양으로 계산합니다.
		if (Spec.Pattern == EAttackAreaPattern::SingleCell)
			Resolved.Pattern = Spec.AnchorMode == EAreaAnchor::UseAnchorCell ? EAttackAreaPattern::CircleOnCell : EAttackAreaPattern::RectFromCell;
	}
	FGridAreaBuilder::BuildCells(Resolved, 9, 5, OutCells);
	if (Spec.Flags == 1) OutCells.Remove(OutCoin);
}

void AWeaponRangePreviewActor::ApplyRange(int32 Group, const FAttackAreaSpec& Spec, bool bEnabled)
{
	TArray<FGridPoint> Cells;
	FGridPoint Coin;
	bool bShowCoin = false;
	const bool bAttack = Group % 2 == 0;
	const EPreviewCoinOrigin* Origin = AbilityCoinOrigins.Find(Spec.Pattern);
	if (bEnabled) BuildPreviewCells(Spec, bAttack, Origin ? *Origin : EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
	const int32 Width = bAttack ? 1 : 9;
	for (int32 Cell = 0; Cell < PreviewCellCounts[Group]; ++Cell)
	{
		const int32 Index = PreviewCellStarts[Group] + Cell;
		if (!PreviewCells.IsValidIndex(Index) || !IsValid(PreviewCells[Index])) continue;
		AGridActor* Grid = Cast<AGridActor>(PreviewCells[Index]->GetChildActor());
		if (!IsValid(Grid)) continue;
		const FGridPoint P(Cell % Width, Cell / Width);
		const bool bCoin = bShowCoin && P == Coin;
		const bool bHighlighted = Cells.Contains(P);
		const FLinearColor Color = bCoin ? CoinCellColor : bHighlighted ?
			(bAttack ? AttackHighlightColor : AbilityHighlightColor) : DefaultCellColor;
		Grid->ApplyCellMaterialParams(Color, bCoin || bHighlighted ? RangeFillIntensity : DefaultFillIntensity, 0.f);
	}
}

void AWeaponRangePreviewActor::ShowDefinitionPreview(const FAttackAreaSpec& FrontAttack,
	const FAttackAreaSpec& FrontAbility, bool bFrontAbility, const FAttackAreaSpec& BackAttack,
	const FAttackAreaSpec& BackAbility, bool bBackAbility)
{
	ApplyRange(0, FrontAttack, true);
	ApplyRange(1, FrontAbility, bFrontAbility);
	ApplyRange(2, BackAttack, true);
	ApplyRange(3, BackAbility, bBackAbility);
	CapturePreviews();
}

void AWeaponRangePreviewActor::ClearPreview()
{
	for (int32 Group = 0; Group < 4; ++Group) ApplyRange(Group, FAttackAreaSpec(), false);
	CapturePreviews();
}

void AWeaponRangePreviewActor::CapturePreviews()
{
	if (!IsValid(GetWorld()) || GetWorld()->bIsTearingDown) return;
	USceneCaptureComponent2D* Captures[] = {FrontAttackCapture, FrontAbilityCapture, BackAttackCapture, BackAbilityCapture};
	for (int32 Group = 0; Group < 4; ++Group)
	{
		USceneCaptureComponent2D* Capture = Captures[Group];
		if (!IsValid(Capture)) continue;
		Capture->bCaptureEveryFrame = false;
		Capture->bCaptureOnMovement = false;
		Capture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		Capture->ShowOnlyActors.Reset();
		Capture->ClearShowOnlyComponents();
		for (int32 Cell = 0; Cell < PreviewCellCounts[Group]; ++Cell)
		{
			const int32 Index = PreviewCellStarts[Group] + Cell;
			if (PreviewCells.IsValidIndex(Index) && IsValid(PreviewCells[Index]))
				if (AActor* Grid = PreviewCells[Index]->GetChildActor(); IsValid(Grid)) Capture->ShowOnlyActorComponents(Grid);
		}
		if (IsValid(Capture->TextureTarget)) Capture->CaptureScene();
	}
}

void AWeaponRangePreviewActor::RebuildGrid()
{
	ConfigureCells();
	ClearPreview();
}

void AWeaponRangePreviewActor::ShowBothFacePreview(const FAttackAreaSpec& FrontSpec, const FAttackAreaSpec& BackSpec)
{
	ShowDefinitionPreview(FrontSpec, FAttackAreaSpec(), false, BackSpec, FAttackAreaSpec(), false);
}

void AWeaponRangePreviewActor::ShowPreview(const FGridPoint& CoinXY, const TArray<FGridPoint>& RangeCells)
{
	// 이전 GridManager 호출이 DB 기본 사거리 화면을 덮어쓰지 않도록 호환 진입점만 유지합니다.
}
