#include "CoinAttackRangeIndicatorActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ACoinAttackRangeIndicatorActor::ACoinAttackRangeIndicatorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRoot"));
	RootComponent = PreviewRoot;

	LineBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineBodyMesh"));
	LineBodyMesh->SetupAttachment(RootComponent);
	LineBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineBodyMesh->SetGenerateOverlapEvents(false);
	LineBodyMesh->SetCastShadow(false);
	LineBodyMesh->SetReceivesDecals(false);
	LineBodyMesh->SetTranslucentSortPriority(100);
	LineBodyMesh->SetVisibility(false);

	EndCapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndCapMesh"));
	EndCapMesh->SetupAttachment(RootComponent);
	EndCapMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EndCapMesh->SetGenerateOverlapEvents(false);
	EndCapMesh->SetCastShadow(false);
	EndCapMesh->SetReceivesDecals(false);
	EndCapMesh->SetTranslucentSortPriority(101);
	EndCapMesh->SetVisibility(false);

	SetActorEnableCollision(false);
}

void ACoinAttackRangeIndicatorActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(EndCapMesh))
	{
		DefaultEndCapRelativeScale = EndCapMesh->GetRelativeScale3D();
	}
}

bool ACoinAttackRangeIndicatorActor::ShowRange(
	const FVector& AttackStartCellWorldLocation,
	const FVector& AttackEndCellWorldLocation,
	const FVector& ForwardWorldDirection)
{
	HideRange();

	if (!IsValid(LineBodyMesh) || !IsValid(EndCapMesh) ||
		!IsValid(LineBodyMesh->GetStaticMesh()) || !IsValid(EndCapMesh->GetStaticMesh()))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AttackRangePreview] LineBodyMesh 또는 EndCapMesh가 BP에 지정되지 않았습니다."));
		return false;
	}

	FVector FlatDirection = ForwardWorldDirection;
	FlatDirection.Z = 0.0f;
	FlatDirection = FlatDirection.GetSafeNormal();

	if (FlatDirection.IsNearlyZero())
	{
		return false;
	}

	const FQuat DirectionRotation = FlatDirection.Rotation().Quaternion();
	// Blender X Forward/Z Up Export 기준의 Body 로컬 +Y를 언리얼 공격 방향(+X)에 맞춥니다.
	const FQuat BodyAxisCorrection = FRotator(0.0f, -90.0f, 0.0f).Quaternion();
	const FQuat FinalRotation =
		DirectionRotation * BodyAxisCorrection * MeshRotationOffset.Quaternion();
	const float GridVisualHalfLength = FMath::Max(1.0f, GridVisualLength) * 0.5f;

	// 셀 중심 간격에는 그리드 사이의 틈이 포함되므로 실제 메시 크기로 경계를 계산합니다.
	const FVector BodyStartWorldLocation =
		AttackStartCellWorldLocation - FlatDirection * GridVisualHalfLength;
	const FVector BodyEndWorldLocation =
		AttackEndCellWorldLocation - FlatDirection * GridVisualHalfLength;
	const FVector EndCapWorldLocation =
		AttackEndCellWorldLocation + FlatDirection * GridVisualHalfLength;
	FVector BaseBodyDirection = BodyEndWorldLocation - BodyStartWorldLocation;
	BaseBodyDirection.Z = 0.0f;
	const bool bEndCapOnly = BaseBodyDirection.IsNearlyZero();

	FVector EndCapScale = DefaultEndCapRelativeScale;
	if (bEndCapOnly)
	{
		EndCapScale.Y = SingleCellEndCapYScale;
	}
	EndCapMesh->SetRelativeScale3D(EndCapScale);

	FVector RaisedEndCapLocation = EndCapWorldLocation;
	RaisedEndCapLocation.Z += PreviewHeightOffset;
	RaisedEndCapLocation.X -= CamOffset;
	// EndCap의 방향은 BP에서 직접 맞추므로 런타임에는 위치만 변경합니다.
	EndCapMesh->SetWorldLocation(RaisedEndCapLocation);
	EndCapMesh->SetVisibility(true);

	// 사거리가 한 칸이면 마지막 칸을 EndCap이 전부 차지하므로 Body는 표시하지 않습니다.
	if (bEndCapOnly)
	{
		return true;
	}

	FVector RaisedStartLocation = BodyStartWorldLocation;
	RaisedStartLocation.Z += PreviewHeightOffset;
	// EndCap 메시의 경계와 살짝 겹치게 해 투명 머테리얼 사이의 이음새가 드러나지 않게 합니다.
	FVector RaisedBodyEndLocation =
		BodyEndWorldLocation + FlatDirection * FMath::Max(0.0f, BodyEndOverlap);
	RaisedBodyEndLocation.Z += PreviewHeightOffset;
	FVector BodyDirection = RaisedBodyEndLocation - RaisedStartLocation;
	BodyDirection.Z = 0.0f;
	const float LineLength = BodyDirection.Size();
	const FVector Midpoint = (RaisedStartLocation + RaisedBodyEndLocation) * 0.5f;
	const float SafeNativeLength = FMath::Max(1.0f, LineBodyNativeLength);

	LineBodyMesh->SetWorldLocationAndRotation(Midpoint, FinalRotation);
	// Blender의 길이 축인 Y만 자동 조절하고, X/Z는 BP에서 지정한 두께를 그대로 유지합니다.
	FVector LineBodyScale = LineBodyMesh->GetRelativeScale3D();
	LineBodyScale.Y = LineLength / SafeNativeLength;
	LineBodyMesh->SetRelativeScale3D(LineBodyScale);
	LineBodyMesh->SetVisibility(true);
	return true;
}

void ACoinAttackRangeIndicatorActor::HideRange()
{
	if (IsValid(LineBodyMesh))
	{
		LineBodyMesh->SetVisibility(false);
	}

	if (IsValid(EndCapMesh))
	{
		EndCapMesh->SetVisibility(false);
	}
}
