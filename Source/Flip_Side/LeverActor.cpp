#include "LeverActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ALeverActor::ALeverActor()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Root);

    LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
    LeverMesh->SetupAttachment(Root);
    
    BaseMesh->SetGenerateOverlapEvents(false);
    LeverMesh->SetGenerateOverlapEvents(true);
}

void ALeverActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 초기 각도
    LeverMesh->SetRelativeRotation(StartRotation);
}

void ALeverActor::OnLeverInteracted()
{
    if (bIsPulled) return;

    PlayLeverAnimation();
    bIsPulled = true;
    
    ExecuteLeverLogic();
}

void ALeverActor::ExecuteLeverLogic()
{
    // 자식 클래스에서 고유 로직 구현
}

void ALeverActor::PlayLeverAnimation()
{
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = GetUniqueID();
    LatentInfo.Linkage = 0;

    UKismetSystemLibrary::MoveComponentTo(
        LeverMesh,
        FVector::ZeroVector,      
        TargetRotation,  
        false,
        true,
        AnimationSpeed, 
        false, 
        EMoveComponentAction::Move, 
        LatentInfo
    );
}