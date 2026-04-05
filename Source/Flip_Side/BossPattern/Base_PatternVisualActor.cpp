#include "BossPattern/Base_PatternVisualActor.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
ABase_PatternVisualActor::ABase_PatternVisualActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ActorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Actor Root"));
	RootComponent = ActorRoot;
	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Actor Mesh"));
	ActorMesh->SetupAttachment(RootComponent);
	MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MovementTimeline"));
}

// Called when the game starts or when spawned
void ABase_PatternVisualActor::BeginPlay()
{
	Super::BeginPlay();

    SetActorLocation(StartLocation);

    if (MovementCurve != nullptr)
    {
        FOnTimelineFloat TimelineProgressDelegate;
        TimelineProgressDelegate.BindUFunction(this, FName("TimelineProgress"));

        FOnTimelineEvent TimelineFinishedDelegate;
        TimelineFinishedDelegate.BindUFunction(this, FName("TimelineFinished"));

        MovementTimeline->AddInterpFloat(MovementCurve, TimelineProgressDelegate);
        MovementTimeline->SetTimelineFinishedFunc(TimelineFinishedDelegate);

    }
}

void ABase_PatternVisualActor::PlayBossPatternAct()
{
    if (TimeBeforeAct > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            WaitTimerHandle, 
            this, 
            &ABase_PatternVisualActor::StartMovement, 
            TimeBeforeAct, 
            false
        );
    }
    else
    {
        StartMovement();
    }
}

void ABase_PatternVisualActor::StartMovement()
{
    if (MovementTimeline)
    {
        MovementTimeline->PlayFromStart();
    }
}

void ABase_PatternVisualActor::TimelineProgress(float Value)
{
    FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Value);
    
    SetActorLocation(NewLocation);
}

void ABase_PatternVisualActor::TimelineFinished()
{
    if (DestroyDelayTime > 0.f)
    {
        SetLifeSpan(DestroyDelayTime);
    }
    else
    {
        Destroy();
    }
}
