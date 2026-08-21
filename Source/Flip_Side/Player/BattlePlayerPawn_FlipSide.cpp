#include "BattlePlayerPawn_FlipSide.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Actors/Boss/BossActor.h"
#include "Subsystem/BattleLevel/BossManagerSubsystem.h"

namespace
{
	struct FBattleSceneSettingPreset
	{
		FVector CameraLocation = FVector::ZeroVector;
		FRotator CameraRotation = FRotator::ZeroRotator;
		FVector CameraScale = FVector::OneVector;
		float CameraFieldOfView = 60.0f;
		FVector BossMeshScale = FVector::OneVector;
	};

	bool TryGetBattleSceneSettingPreset(int32 Version, FBattleSceneSettingPreset& OutPreset)
	{
		switch (Version)
		{
		case 0:
			OutPreset.CameraLocation = FVector(0.0f, 200.0f, 1000.0f);
			OutPreset.CameraRotation = FRotator(-8.0f, 0.0f, 0.0f);
			OutPreset.CameraScale = FVector::OneVector;
			OutPreset.CameraFieldOfView = 60.0f;
			OutPreset.BossMeshScale = FVector(20.0f, 20.0f, 20.0f);
			return true;
		case 1:
			OutPreset.CameraLocation = FVector(-852.0f, 200.0f, 2785.0f);
			OutPreset.CameraRotation = FRotator(-21.0f, 0.0f, 0.0f);
			OutPreset.CameraScale = FVector::OneVector;
			OutPreset.CameraFieldOfView = 55.0f;
			OutPreset.BossMeshScale = FVector(12.0f, 12.0f, 12.0f);
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
	Camera->SetRelativeLocation(FVector(0.0f, 200.0f, 1000.0f));
	Camera->SetRelativeRotation(FRotator(-8.0f, 0.0f, 0.0f));

	// Perspective 모드 for 시각적 깊이감
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;
	Camera->FieldOfView = 60.0f;

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

void ABattlePlayerPawn_FlipSide::SetCamSettingVer(int32 Version)
{
	FBattleSceneSettingPreset Preset;
	if (!TryGetBattleSceneSettingPreset(Version, Preset))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerPawn] SetCamSettingVer: 지원하지 않는 버전입니다. Version=%d"), Version);
		return;
	}

	if (!IsValid(Camera))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerPawn] SetCamSettingVer: Camera가 유효하지 않습니다."));
		return;
	}

	Camera->SetRelativeLocation(Preset.CameraLocation);
	Camera->SetRelativeRotation(Preset.CameraRotation);
	Camera->SetRelativeScale3D(Preset.CameraScale);
	Camera->SetProjectionMode(ECameraProjectionMode::Perspective);
	Camera->SetFieldOfView(Preset.CameraFieldOfView);

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

	Boss->BossMesh->SetRelativeScale3D(Preset.BossMeshScale);
	UE_LOG(LogTemp, Log, TEXT("[BattlePlayerPawn] SetCamSettingVer %d 적용 완료"), Version);
}

void ABattlePlayerPawn_FlipSide::MoveCameraToArea(FVector NewTargetLocation, FRotator NewTargetRotation, float NewTargetArmLength)
{
    TargetLocation = NewTargetLocation;
    TargetRotation = NewTargetRotation;
    TargetArmLength = NewTargetArmLength;
}
