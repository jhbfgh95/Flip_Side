#include "AbilityRangeActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"

AAbilityRangeActor::AAbilityRangeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRoot"));
	RootComponent = PreviewRoot;

	RangePlaneInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RangePlaneInstances"));
	RangePlaneInstances->SetupAttachment(RootComponent);
	RangePlaneInstances->SetMobility(EComponentMobility::Movable);
	RangePlaneInstances->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangePlaneInstances->SetGenerateOverlapEvents(false);
	RangePlaneInstances->SetCastShadow(false);
	RangePlaneInstances->SetReceivesDecals(false);
	RangePlaneInstances->SetCanEverAffectNavigation(false);
	RangePlaneInstances->SetVisibility(false);

	SetActorEnableCollision(false);
}

void AAbilityRangeActor::ShowRangeAtWorldLocations(const TArray<FVector>& WorldLocations)
{
	HideRange();

	if (!IsValid(RangePlaneInstances) || !IsValid(RangePlaneInstances->GetStaticMesh()))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AbilityRangePreview] RangePlaneInstances의 Plane Mesh가 BP에 지정되지 않았습니다."));
		return;
	}

	for (const FVector& WorldLocation : WorldLocations)
	{
		FVector RaisedLocation = WorldLocation;
		RaisedLocation.Z += PlaneHeightOffset;
		RangePlaneInstances->AddInstance(
			FTransform(PlaneRotation, RaisedLocation, PlaneScale),
			true
		);
	}

	RangePlaneInstances->SetVisibility(RangePlaneInstances->GetInstanceCount() > 0);
}

void AAbilityRangeActor::HideRange()
{
	if (!IsValid(RangePlaneInstances))
	{
		return;
	}

	RangePlaneInstances->ClearInstances();
	RangePlaneInstances->SetVisibility(false);
}
