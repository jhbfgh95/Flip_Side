#include "DrawActor.h"
#include "BattleLevelActingWSubsystem.h"
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

	UBattleLevelActingWSubsystem* ActManager= GetWorld()->GetSubsystem<UBattleLevelActingWSubsystem>();
	ActManager->DoDrawMove.BindUObject(this, &ADrawActor::DoDrawAct);

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

	GetWorld()->GetTimerManager().SetTimer(MovementTimer, this, &ADrawActor::WaitForLever, 0.6f, false);
}

void ADrawActor::WaitForLever()
{
	GetWorld()->GetTimerManager().SetTimer(MovementTimer, this, &ADrawActor::DrawAct, 0.01f, true);
}

void ADrawActor::DrawAct()
{
	//밖에서 안으로
	if(bIsDrawOut)
	{
		MoveElapsedTime += 0.01f;
		float Alpha = FMath::Clamp(MoveElapsedTime / MoveTime, 0.0f, 1.0f);

		float NewX = FMath::Lerp(TargetOutXLoc, TargetInXLoc, Alpha);

		FVector CurrentLoc = GetActorLocation();
		SetActorLocation(FVector(NewX, CurrentLoc.Y, CurrentLoc.Z));

		if(Alpha >= 1.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
		}
	}
	else //안에서 밖으로 -> 이건MoveTime 바꿀지도
	{
		MoveElapsedTime += 0.01f;
		float Alpha = FMath::Clamp(MoveElapsedTime / MoveTime, 0.0f, 1.0f);

		float NewX = FMath::Lerp(TargetInXLoc, TargetOutXLoc, Alpha);

		FVector CurrentLoc = GetActorLocation();
		SetActorLocation(FVector(NewX, CurrentLoc.Y, CurrentLoc.Z));

		if(Alpha >= 1.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
		}
	}

}