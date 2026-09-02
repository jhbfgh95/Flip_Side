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
	const FQuat FinalRotation = DirectionRotation * MeshRotationOffset.Quaternion();
	const float GridVisualHalfLength = FMath::Max(1.0f, GridVisualLength) * 0.5f;

	// 셀 중심 간격에는 그리드 사이의 틈이 포함되므로 실제 메시 크기로 경계를 계산합니다.
	const FVector BodyStartWorldLocation =
		AttackStartCellWorldLocation - FlatDirection * GridVisualHalfLength;
	const FVector BodyEndWorldLocation =
		AttackEndCellWorldLocation - FlatDirection * GridVisualHalfLength;
	const FVector EndCapWorldLocation =
		AttackEndCellWorldLocation + FlatDirection * GridVisualHalfLength;
	FVector BodyDirection = BodyEndWorldLocation - BodyStartWorldLocation;
	BodyDirection.Z = 0.0f;
	const float LineLength = BodyDirection.Size();

	FVector EndCapScale = DefaultEndCapRelativeScale;
	if (LineLength <= KINDA_SMALL_NUMBER)
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
	if (LineLength <= KINDA_SMALL_NUMBER)
	{
		return true;
	}

	FVector RaisedStartLocation = BodyStartWorldLocation;
	RaisedStartLocation.Z += PreviewHeightOffset;
	FVector RaisedBodyEndLocation = BodyEndWorldLocation;
	RaisedBodyEndLocation.Z += PreviewHeightOffset;
	const FVector Midpoint = (RaisedStartLocation + RaisedBodyEndLocation) * 0.5f;
	const float SafeNativeLength = FMath::Max(1.0f, LineBodyNativeLength);

	LineBodyMesh->SetWorldLocationAndRotation(Midpoint, FinalRotation);
	// X는 사거리에 맞게 자동 조절하고, Y/Z는 BP 컴포넌트에서 지정한 두께를 그대로 유지합니다.
	FVector LineBodyScale = LineBodyMesh->GetRelativeScale3D();
	LineBodyScale.X = LineLength / SafeNativeLength;
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
