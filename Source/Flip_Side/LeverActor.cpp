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
    if (bIsPulled) return; 
    bIsPulled = true;

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = GetUniqueID();
    LatentInfo.Linkage = 0;

    // 레버 내려가는 연출
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

    // 레버 다시 원래대로
    float ReturnDelay = 2.0f; 
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        FLatentActionInfo ReturnInfo;
        ReturnInfo.CallbackTarget = this;
        ReturnInfo.UUID = GetUniqueID() + 1;
        
        UKismetSystemLibrary::MoveComponentTo(
            LeverMesh, 
            FVector::ZeroVector, 
            StartRotation, 
            false, 
            true, 
            AnimationSpeed, 
            false, 
            EMoveComponentAction::Move, 
            ReturnInfo
        );

        bIsPulled = false; 
    }, ReturnDelay, false);
}