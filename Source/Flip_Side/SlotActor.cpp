#include "SlotActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ASlotActor::ASlotActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SlotRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = SlotRootComponent;

	SlotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Slot Mesh Component"));
	SlotMesh->SetupAttachment(RootComponent);

	CoinSlotLocation = CreateDefaultSubobject<USceneComponent>(TEXT("CoinSlotLocation"));
	CoinSlotLocation->SetupAttachment(RootComponent);
}

void ASlotActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASlotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform ASlotActor::GetSlotTransform() const
{
	return CoinSlotLocation->GetComponentTransform();
}

int32 ASlotActor::GetSlotIndex() const
{
	return SlotIndex;
}