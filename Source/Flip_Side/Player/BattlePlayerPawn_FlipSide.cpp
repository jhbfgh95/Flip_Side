#include "BattlePlayerPawn_FlipSide.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Actors/Boss/BossActor.h"
#include "Subsystem/BattleLevel/BossManagerSubsystem.h"

namespace
{
	struct FCameraSettingPreset
	{
		FVector RelativeLocation = FVector::ZeroVector;
		FRotator RelativeRotation = FRotator::ZeroRotator;
		FVector RelativeScale3D = FVector::OneVector;
		float FieldOfView = 90.0f;
	};

	bool TryGetCameraSettingPreset(int32 Version, FCameraSettingPreset& OutPreset)
	{
		switch (Version)
		{
		case 0:
			OutPreset.RelativeLocation = FVector(-852.0f, 200.0f, 2785.0f);
			OutPreset.RelativeRotation = FRotator(-21.0f, 0.0f, 0.0f);
			OutPreset.FieldOfView = 55.0f;
			return true;
		case 1:
			OutPreset.RelativeLocation = FVector(-2354.0f, 242.0f, 3296.0f);
			OutPreset.RelativeRotation = FRotator(-20.0f, 0.0f, 0.0f);
			OutPreset.FieldOfView = 50.0f;
			return true;
		case 2:
			OutPreset.RelativeLocation = FVector(-3036.0f, 242.0f, 3412.0f);
			OutPreset.RelativeRotation = FRotator(-19.0f, 0.0f, 0.0f);
			OutPreset.RelativeScale3D = FVector::OneVector;
			OutPreset.FieldOfView = 45.0f;
			return true;
		default:
			return false;
		}
	}
}

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
	Camera->SetRelativeLocation(FVector(-852.0f, 200.0f, 2785.0f));
	Camera->SetRelativeRotation(FRotator(-21.0f, 0.0f, 0.0f));
	Camera->SetRelativeScale3D(FVector::OneVector);

	// Perspective 모드 for 시각적 깊이감
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;
	Camera->FieldOfView = 55.0f;

	// 초기 목표 상태 설정
	TargetLocation = GetActorLocation();
	TargetRotation = SpringArm->GetRelativeRotation();
	TargetArmLength = SpringArm->TargetArmLength;
}

void ABattlePlayerPawn_FlipSide::BeginPlay()
{
	Super::BeginPlay();
	
	// 고정 좌푯값
    TargetLocation = FVector(-3086.0f, -990.0f, 2438.0f); 
    TargetRotation = FRotator(-21.6f, 0.0f, 0.0f);
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

void ABattlePlayerPawn_FlipSide::SetCamSettingVer(int32 Version, float BossScaleXYZ)
{
	FCameraSettingPreset Preset;
	if (!TryGetCameraSettingPreset(Version, Preset))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerPawn] SetCamSettingVer: 지원하지 않는 버전입니다. Version=%d"), Version);
		return;
	}

	if (!IsValid(Camera))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerPawn] SetCamSettingVer: Camera가 유효하지 않습니다."));
		return;
	}

	Camera->SetRelativeLocation(Preset.RelativeLocation);
	Camera->SetRelativeRotation(Preset.RelativeRotation);
	Camera->SetRelativeScale3D(Preset.RelativeScale3D);
	Camera->SetFieldOfView(Preset.FieldOfView);

	UWorld* World = GetWorld();
	UBossManagerSubsystem* BossManager = IsValid(World) ? World->GetSubsystem<UBossManagerSubsystem>() : nullptr;
	ABossActor* Boss = IsValid(BossManager) ? BossManager->GetCurrentBoss() : nullptr;
	if (!IsValid(Boss) || !IsValid(Boss->BossMesh))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BattlePlayerPawn] SetCamSettingVer %d: 카메라는 적용했지만 현재 BossMesh가 유효하지 않습니다."),
			Version);
		return;
	}

	Boss->BossMesh->SetRelativeScale3D(FVector(BossScaleXYZ));
	UE_LOG(LogTemp, Log,
		TEXT("[BattlePlayerPawn] SetCamSettingVer %d 적용 완료, BossScaleXYZ=%.3f"),
		Version,
		BossScaleXYZ);
}

void ABattlePlayerPawn_FlipSide::MoveCameraToArea(FVector NewTargetLocation, FRotator NewTargetRotation, float NewTargetArmLength)
{
    TargetLocation = NewTargetLocation;
    TargetRotation = NewTargetRotation;
    TargetArmLength = NewTargetArmLength;
}
