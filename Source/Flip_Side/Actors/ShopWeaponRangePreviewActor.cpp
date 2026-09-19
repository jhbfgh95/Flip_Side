#include "ShopWeaponRangePreviewActor.h"
#include "GridActor.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"

namespace
{
	constexpr int32 ShopPreviewCellStarts[] = {0, 6};
	constexpr int32 ShopPreviewCellCounts[] = {6, 45};
}

AShopWeaponRangePreviewActor::AShopWeaponRangePreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRoot"));
	AttackPreviewRangeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AttackPreviewRangeComponent"));
	AbilityPreviewRangeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AbilityPreviewRangeComponent"));
	AttackCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("AttackCapture"));
	AbilityCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("AbilityCapture"));
	USceneComponent* Groups[] = {AttackPreviewRangeComponent, AbilityPreviewRangeComponent};
	USceneCaptureComponent2D* Captures[] = {AttackCapture, AbilityCapture};
	for (int32 Group = 0; Group < 2; ++Group)
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
		for (int32 Cell = 0; Cell < ShopPreviewCellCounts[Group]; ++Cell)
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

void AShopWeaponRangePreviewActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ConfigureCells();
}

void AShopWeaponRangePreviewActor::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCells();
	if (!GridActorClass)
		UE_LOG(LogTemp, Warning, TEXT("ShopWeaponRangePreviewActor: assign GridActorClass (BP_Grid) in the preview BP."));
	USceneCaptureComponent2D* Captures[] = {AttackCapture, AbilityCapture};
	TSet<UTextureRenderTarget2D*> Targets;
	for (USceneCaptureComponent2D* Capture : Captures)
	{
		if (!IsValid(Capture)) continue;
		if (!IsValid(Capture->TextureTarget))
		{
			UE_LOG(LogTemp, Warning, TEXT("ShopWeaponRangePreviewActor: assign TextureTarget on %s."), *Capture->GetName());
		}
		else if (Targets.Contains(Capture->TextureTarget))
		{
			UE_LOG(LogTemp, Warning, TEXT("ShopWeaponRangePreviewActor: each capture needs a DIFFERENT TextureTarget (%s)."), *Capture->GetName());
		}
		else Targets.Add(Capture->TextureTarget);
	}
	ClearPreview();
}

void AShopWeaponRangePreviewActor::ConfigureCells()
{
	for (int32 Group = 0; Group < 2; ++Group)
	{
		const int32 Width = Group % 2 == 0 ? 1 : 9;
		for (int32 Cell = 0; Cell < ShopPreviewCellCounts[Group]; ++Cell)
		{
			const int32 Index = ShopPreviewCellStarts[Group] + Cell;
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

void AShopWeaponRangePreviewActor::ApplyRange(int32 Group, const FAttackAreaSpec& Spec, bool bEnabled)
{
	TArray<FGridPoint> Cells;
	FGridPoint Coin;
	bool bShowCoin = false;
	const bool bAttack = Group % 2 == 0;
	const EPreviewCoinOrigin* Origin = AbilityCoinOrigins.Find(Spec.Pattern);
	// 배틀과 동일한 순수 계산 함수를 공유하며 배틀 액터 인스턴스는 필요하지 않습니다.
	if (bEnabled) AWeaponRangePreviewActor::BuildPreviewCells(Spec, bAttack, Origin ? *Origin : EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
	const int32 Width = bAttack ? 1 : 9;
	for (int32 Cell = 0; Cell < ShopPreviewCellCounts[Group]; ++Cell)
	{
		const int32 Index = ShopPreviewCellStarts[Group] + Cell;
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

void AShopWeaponRangePreviewActor::ShowWeaponDefinition(const FFaceData& WeaponDefinition)
{
	// DB 정의의 초기 Spec만 전달합니다. 버프/런타임 코인 스냅숏은 받지 않습니다.
	ShowDefinitionPreview(WeaponDefinition.AttackAreaSpec, WeaponDefinition.AbilityAreaSpec, WeaponDefinition.bHasAbilityArea);
}

void AShopWeaponRangePreviewActor::ShowDefinitionPreview(const FAttackAreaSpec& AttackSpec,
	const FAttackAreaSpec& AbilitySpec, bool bHasAbilityArea)
{
	ApplyRange(0, AttackSpec, true);
	ApplyRange(1, AbilitySpec, bHasAbilityArea);
	CapturePreviews();
}

void AShopWeaponRangePreviewActor::ClearPreview()
{
	for (int32 Group = 0; Group < 2; ++Group) ApplyRange(Group, FAttackAreaSpec(), false);
	CapturePreviews();
}

void AShopWeaponRangePreviewActor::CapturePreviews()
{
	if (!IsValid(GetWorld()) || GetWorld()->bIsTearingDown) return;
	USceneCaptureComponent2D* Captures[] = {AttackCapture, AbilityCapture};
	for (int32 Group = 0; Group < 2; ++Group)
	{
		USceneCaptureComponent2D* Capture = Captures[Group];
		if (!IsValid(Capture)) continue;
		Capture->bCaptureEveryFrame = false;
		Capture->bCaptureOnMovement = false;
		Capture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		Capture->ShowOnlyActors.Reset();
		Capture->ClearShowOnlyComponents();
		for (int32 Cell = 0; Cell < ShopPreviewCellCounts[Group]; ++Cell)
		{
			const int32 Index = ShopPreviewCellStarts[Group] + Cell;
			if (PreviewCells.IsValidIndex(Index) && IsValid(PreviewCells[Index]))
				if (AActor* Grid = PreviewCells[Index]->GetChildActor(); IsValid(Grid)) Capture->ShowOnlyActorComponents(Grid);
		}
		if (IsValid(Capture->TextureTarget)) Capture->CaptureScene();
	}
}

void AShopWeaponRangePreviewActor::RebuildGrid()
{
	ConfigureCells();
	ClearPreview();
}
