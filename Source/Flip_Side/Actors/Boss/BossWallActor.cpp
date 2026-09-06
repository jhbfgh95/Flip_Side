#include "BossWallActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ABossWallActor::ABossWallActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(RootComponent);
	WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossWallActor::SetBossAreaSize(const FVector2D& InSize)
{
	BossAreaSize = InSize;
	UpdateWallTransform();
}

void ABossWallActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateWallTransform();
}

void ABossWallActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateWallTransform();
}

#if WITH_EDITOR
void ABossWallActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateWallTransform();
}
#endif

void ABossWallActor::UpdateWallTransform()
{
	if (!IsValid(WallMesh) || !IsValid(WallMesh->GetStaticMesh())) return;
	const FBox Bounds = WallMesh->GetStaticMesh()->GetBoundingBox();
	const FVector MeshSize = Bounds.GetSize();
	if (MeshSize.X <= UE_SMALL_NUMBER || MeshSize.Y <= UE_SMALL_NUMBER || MeshSize.Z <= UE_SMALL_NUMBER) return;

	const FVector TargetSize(
		FMath::Max(1.0, BossAreaSize.X + 2.0 * FMath::Max(0.f, MarginX)),
		FMath::Max(1.0, BossAreaSize.Y + 2.0 * FMath::Max(0.f, MarginY)),
		FMath::Max(1.f, WallHeight));
	const FVector Scale = TargetSize / MeshSize;
	const FVector Center = Bounds.GetCenter();
	WallMesh->SetRelativeRotation(FRotator::ZeroRotator);
	WallMesh->SetRelativeScale3D(Scale);
	WallMesh->SetRelativeLocation(FVector(-Center.X * Scale.X, -Center.Y * Scale.Y, -Bounds.Min.Z * Scale.Z));
}
