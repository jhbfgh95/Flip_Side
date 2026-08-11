#include "BossCoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

ABossCoinActor::ABossCoinActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossCoinRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = BossCoinRoot;

	BossCoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boss Coin Mesh"));
	BossCoinMesh->SetupAttachment(RootComponent);
	BossCoinMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
}

void ABossCoinActor::BeginPlay()
{
	Super::BeginPlay();
}
