#include "BattlePlayerPawn_FlipSide.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABattlePlayerPawn_FlipSide::ABattlePlayerPawn_FlipSide()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 스프링 암
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bDoCollisionTest = false; // 카메라가 사물에 부딪혀 튀는 현상 방지
    SpringArm->TargetArmLength = 1000.0f; // 초기 거리 설정
    SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f)); // 초기 각도 설정

	// 카메라, 스프링 암 끝에
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Perspective 모드 for 시각적 깊이감
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;

	// 초기 목표 상태 설정
	TargetLocation = GetActorLocation();
	TargetRotation = SpringArm->GetRelativeRotation();
	TargetArmLength = SpringArm->TargetArmLength;
}

void ABattlePlayerPawn_FlipSide::BeginPlay()
{
	Super::BeginPlay();
	
	// 고정 좌푯값
    TargetLocation = FVector(-1820.0f, -990.0f, 2220.0f); 
    TargetRotation = FRotator(-35.0f, 0.0f, 0.0f);
    TargetArmLength = 0.0f; 

    // 게임 시작 시 Pawn을 즉시 그 위치로 설정
    SetActorLocation(TargetLocation);
    SpringArm->SetRelativeRotation(TargetRotation);
    SpringArm->TargetArmLength = TargetArmLength;
}

void ABattlePlayerPawn_FlipSide::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 프레임마다 목표 위치로 부드럽게 이동
	// 위치 이동 (Pawn 위치)
	FVector CurrentLocation = GetActorLocation();
    if (!CurrentLocation.Equals(TargetLocation, 0.1f))
    {
        SetActorLocation(FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, InterpolationSpeed));
    }
	// 회전 이동 (SpringArm 회전)
	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
    if (!CurrentRotation.Equals(TargetRotation, 0.1f))
    {
        SpringArm->SetRelativeRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpolationSpeed));
    }
	// 거리 조절 (SpringArm 길이)
	if (FMath::Abs(SpringArm->TargetArmLength - TargetArmLength) > 0.1f)
    {
        SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, InterpolationSpeed);
    }
}

void ABattlePlayerPawn_FlipSide::MoveCameraToArea(FVector NewTargetLocation, FRotator NewTargetRotation, float NewTargetArmLength)
{
    TargetLocation = NewTargetLocation;
    TargetRotation = NewTargetRotation;
    TargetArmLength = NewTargetArmLength;
}