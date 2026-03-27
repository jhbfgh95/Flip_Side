#include "Base_OtherActor.h"

ABase_OtherActor::ABase_OtherActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABase_OtherActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABase_OtherActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase_OtherActor::OnClicked_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("DFSAD"));
}