#include "DrawActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ADrawActor::ADrawActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DrawRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = DrawRootComp;

	DrawMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Draw Mesh"));
	DrawMesh->SetupAttachment(RootComponent);
}

void ADrawActor::BeginPlay()
{
	Super::BeginPlay();

	// ReadyCoinWidget으로 대체되었습니다. 맵의 기존 BP 에셋 호환을 위해 Actor 클래스만 유지합니다.
	OriginDrawLocation = GetActorLocation();
}

// Called every frame
void ADrawActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADrawActor::DoDrawAct(bool bIsOut)
{
	GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
	GetWorld()->GetTimerManager().ClearTimer(LeverTimer);

	bIsDrawOut = bIsOut;

	MoveElapsedTime = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(MovementTimer, this, &ADrawActor::WaitForLever, 0.6f, false);
}

void ADrawActor::WaitForLever()
{
	GetWorld()->GetTimerManager().SetTimer(MovementTimer, this, &ADrawActor::DrawAct, 0.01f, true);
}

void ADrawActor::DrawAct()
{
	MoveElapsedTime += 0.01f;
	float Alpha = FMath::Clamp(MoveElapsedTime / MoveTime, 0.0f, 1.0f);

	float NewX = 0.0f;

	if(bIsDrawOut) // 밖에서 안으로 들어감
	{
		NewX = FMath::Lerp(TargetOutXLoc, TargetInXLoc, Alpha);
	}
	else // 안에서 밖으로 나옴
	{
		NewX = FMath::Lerp(TargetInXLoc, TargetOutXLoc, Alpha);
	}

	FVector CurrentLoc = GetActorLocation();
	SetActorLocation(FVector(NewX, CurrentLoc.Y, CurrentLoc.Z));

	if(Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
	}
}
