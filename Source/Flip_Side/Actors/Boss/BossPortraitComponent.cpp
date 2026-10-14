// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Boss/BossPortraitComponent.h"
#include "Actors/Boss/BossActor.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

UBossPortraitComponent::UBossPortraitComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 애니메이션이 갱신된 뒤에 머리 위치를 읽습니다.
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;

	// 매 프레임이 아니라 CaptureRate에 맞춰 CaptureScene()으로 찍습니다. 프레임 사이의 조명 이력(Lumen 등)은 유지합니다.
	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;
	bAlwaysPersistRenderingState = true;
	FOVAngle = 25.f;
	// 색은 RGB에, 투명도는 A에 담깁니다(배경이 비어 있으면 투명).
	CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	// 작은 창에 필요 없는 것은 끕니다. 그림자는 얼굴 입체감을 위해 그대로 둡니다.
	ShowFlags.SetAtmosphere(false);
	ShowFlags.SetFog(false);
	ShowFlags.SetVolumetricFog(false);
	ShowFlags.SetMotionBlur(false);
	ShowFlags.SetBloom(false);
	ShowFlags.SetAmbientOcclusion(false);
	ShowFlags.SetScreenSpaceReflections(false);
	ShowFlags.SetPostProcessing(false);
}

void UBossPortraitComponent::BeginPlay()
{
	Super::BeginPlay();

	ABossActor* Boss = Cast<ABossActor>(GetOwner());
	TargetMesh = Boss ? Boss->BossMesh : nullptr;
	if (!IsValid(TargetMesh))
	{
		SetComponentTickEnabled(false);
		return;
	}

	// 보스 몸체만 찍습니다. 바닥·이펙트·다른 액터는 그리지 않습니다.
	ShowOnlyComponent(TargetMesh);
	PortraitRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, RenderTargetSize, RenderTargetSize, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::Transparent);
	TextureTarget = PortraitRenderTarget;
	Boss->OnBossDead.AddUniqueDynamic(this, &UBossPortraitComponent::HandleBossDead);
	ResolveBones();
}

void UBossPortraitComponent::HandleBossDead()
{
	SetComponentTickEnabled(false);
}

bool UBossPortraitComponent::ResolveBones()
{
	if (!IsValid(TargetMesh))
	{
		return false;
	}

	if (HeadBoneName.IsNone())
	{
		// Meshy 계열 리그(Head/head_end)와 튜토리얼 보스 리그(face/face_end) 순서로 찾습니다.
		static const TCHAR* Candidates[][2] = {
			{ TEXT("Head"), TEXT("head_end") },
			{ TEXT("face"), TEXT("face_end") },
		};
		for (const auto& Candidate : Candidates)
		{
			if (TargetMesh->DoesSocketExist(FName(Candidate[0])))
			{
				HeadBoneName = FName(Candidate[0]);
				if (HeadTopBoneName.IsNone()) HeadTopBoneName = FName(Candidate[1]);
				break;
			}
		}
	}

	return !HeadBoneName.IsNone() && TargetMesh->DoesSocketExist(HeadBoneName);
}

bool UBossPortraitComponent::GetHeadFrame(FVector& OutCenter, float& OutHeight) const
{
	if (!IsValid(TargetMesh))
	{
		return false;
	}

	if (Framing == EBossPortraitFraming::UpperBody)
	{
		const FBoxSphereBounds& MeshBounds = TargetMesh->Bounds;
		OutCenter = MeshBounds.Origin + FVector::UpVector * MeshBounds.BoxExtent.Z * 0.5f;
		OutHeight = FMath::Max(MeshBounds.BoxExtent.Z, 1.f);
		return true;
	}

	if (HeadBoneName.IsNone() || !TargetMesh->DoesSocketExist(HeadBoneName))
	{
		return false;
	}

	const FVector HeadLocation = TargetMesh->GetSocketLocation(HeadBoneName);
	OutCenter = HeadLocation;
	OutHeight = 0.f;
	if (!HeadTopBoneName.IsNone() && TargetMesh->DoesSocketExist(HeadTopBoneName))
	{
		const FVector TopLocation = TargetMesh->GetSocketLocation(HeadTopBoneName);
		OutCenter = FMath::Lerp(HeadLocation, TopLocation, FMath::Clamp(HeadCenterRatio, 0.f, 1.f));
		OutHeight = (TopLocation - HeadLocation).Size();
	}
	OutCenter += FVector::UpVector * VerticalOffsetInMeshUnits * TargetMesh->GetComponentScale().GetAbsMax();
	return true;
}

void UBossPortraitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(TargetMesh) || !IsValid(PortraitRenderTarget) || !GetOwner())
	{
		return;
	}
	if (Framing == EBossPortraitFraming::HeadBones && HeadBoneName.IsNone() && !ResolveBones())
	{
		return;
	}

	FVector HeadCenter;
	float HeadHeight = 0.f;
	if (!GetHeadFrame(HeadCenter, HeadHeight))
	{
		return;
	}

	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	if (!bHasRestDepth)
	{
		// 보스는 플레이어 쪽을 보고 서 있으므로, 머리에서 플레이어 카메라 쪽 수평 방향을 얼굴 앞으로 봅니다. 한 번 정한 뒤 고정합니다.
		FVector Forward = GetOwner()->GetActorForwardVector();
		if (const APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(this, 0))
		{
			const FVector ToPlayer = (PlayerCamera->GetCameraLocation() - HeadCenter).GetSafeNormal2D();
			if (!ToPlayer.IsNearlyZero())
			{
				Forward = ToPlayer;
			}
		}
		FaceForward = Forward.RotateAngleAxis(YawOffset, FVector::UpVector).GetSafeNormal2D();
		if (FaceForward.IsNearlyZero())
		{
			FaceForward = FVector::ForwardVector;
		}
		RestDepth = FVector::DotProduct(HeadCenter - OwnerLocation, FaceForward);
		bHasRestDepth = true;
	}

	// 머리 높이가 화면의 HeadFillRatio만큼 차도록 거리를 정합니다. 머리 크기를 모르면 고정 거리를 씁니다.
	float Distance = DistanceInMeshUnits * TargetMesh->GetComponentScale().GetAbsMax();
	if (HeadHeight > KINDA_SMALL_NUMBER)
	{
		const float HalfFrameHeight = HeadHeight / (2.f * FMath::Max(HeadFillRatio, 0.1f));
		Distance = HalfFrameHeight / FMath::Tan(FMath::DegreesToRadians(FOVAngle * 0.5f));
	}

	// 앞뒤는 처음 자리에 고정하고 좌우·상하만 머리를 따라갑니다. 카메라는 지면과 수평입니다.
	const FVector Offset = HeadCenter - OwnerLocation;
	const FVector Lateral = Offset - FaceForward * FVector::DotProduct(Offset, FaceForward);
	const FVector CameraLocation = OwnerLocation + Lateral + FaceForward * (RestDepth + Distance);

	FRotator CameraRotation = (-FaceForward).Rotation();
	CameraRotation.Pitch += PitchOffset;
	SetWorldLocationAndRotation(CameraLocation, CameraRotation);

	CaptureAccumulator += DeltaTime;
	if (CaptureAccumulator >= 1.f / FMath::Max(CaptureRate, 1.f))
	{
		CaptureAccumulator = 0.f;
		CaptureScene();
	}
}
