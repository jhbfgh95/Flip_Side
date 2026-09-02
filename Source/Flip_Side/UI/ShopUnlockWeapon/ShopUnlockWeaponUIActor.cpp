// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/ShopUnlockWeaponUIActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"

AShopUnlockWeaponUIActor::AShopUnlockWeaponUIActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRoot"));
	RootComponent = PreviewRoot;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(PreviewRoot);

	UnlockWeaponEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UnlockWeaponEffect"));
	UnlockWeaponEffect->SetupAttachment(PreviewRoot);
	UnlockWeaponEffect->SetAutoActivate(false);

	//ShakeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ShakeTimeline"));
	HoldShakeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("HoldShakeTimeline"));

}

void AShopUnlockWeaponUIActor::BeginPlay()
{
	Super::BeginPlay();
	/*
	if (IsValid(ShakeTimeline) && IsValid(ShakeCurve))
	{
		FOnTimelineFloat UpdateCallback;
		UpdateCallback.BindUFunction(this, FName("UpdateShakeMovement"));
		ShakeTimeline->AddInterpFloat(ShakeCurve, UpdateCallback);

		FOnTimelineEvent FinishedCallback;
		FinishedCallback.BindUFunction(this, FName("FinishShakeMovement"));
		ShakeTimeline->SetTimelineFinishedFunc(FinishedCallback);
	}*/

	UCurveFloat* HoldCurve = IsValid(HoldShakeCurve) ? HoldShakeCurve : ShakeCurve;
	if (IsValid(HoldShakeTimeline) && IsValid(HoldCurve))
	{
		FOnTimelineFloat HoldUpdateCallback;
		HoldUpdateCallback.BindUFunction(this, FName("UpdateHoldShakeMovement"));
		HoldShakeTimeline->AddInterpFloat(HoldCurve, HoldUpdateCallback);
		HoldShakeTimeline->SetLooping(true);
	}
}

void AShopUnlockWeaponUIActor::SetWeaponIcon(const FFaceData& InWeaponData)
{
	if (InWeaponData.WeaponID == -1)
	{
		ResetWeaponIcon();
		return;
	}

	SelectedWeaponData = InWeaponData;

	if (!IsValid(WeaponMesh))
	{
		return;
	}

	if (!IsValid(WeaponMaterialInstance))
	{
		WeaponMaterialInstance = WeaponMesh->CreateDynamicMaterialInstance(0);
	}

	if (IsValid(WeaponMaterialInstance))
	{
		WeaponMaterialInstance->SetTextureParameterValue(FName("Front_Texture"), SelectedWeaponData.WeaponIcon);
		WeaponMaterialInstance->SetVectorParameterValue(FName("Front_Color"), TypeColor);
	}

}

void AShopUnlockWeaponUIActor::ResetWeaponIcon()
{
	SelectedWeaponData = FFaceData();
	SelectedWeaponData.WeaponID = -1;

	if (IsValid(WeaponMaterialInstance))
	{
		WeaponMaterialInstance->SetTextureParameterValue(FName("Front_Texture"), nullptr);
		WeaponMaterialInstance->SetVectorParameterValue(FName("Front_Color"), FLinearColor::Black);
	}
}

bool AShopUnlockWeaponUIActor::PlayBuyWeaponAnim()
{
	StopHoldShake();

	if (!IsValid(ShakeTimeline) || !IsValid(ShakeCurve) || !IsValid(WeaponMesh))
	{
		return false;
	}

	WeaponMeshStartLocation = WeaponMesh->GetRelativeLocation();
	ShakeTimeline->PlayFromStart();
	return true;
}

void AShopUnlockWeaponUIActor::StartHoldShake()
{
	if (bIsHoldShaking || !IsValid(HoldShakeTimeline) || !IsValid(WeaponMesh))
		return;

	WeaponMeshStartLocation = WeaponMesh->GetRelativeLocation();
	bIsHoldShaking = true;
	HoldShakeTimeline->PlayFromStart();
}

void AShopUnlockWeaponUIActor::StopHoldShake()
{
	if (!bIsHoldShaking)
		return;

	if (IsValid(HoldShakeTimeline))
		HoldShakeTimeline->Stop();

	if (IsValid(WeaponMesh))
		WeaponMesh->SetRelativeLocation(WeaponMeshStartLocation);

	bIsHoldShaking = false;
}

void AShopUnlockWeaponUIActor::CompleteHoldShake()
{
	StopHoldShake();
	FinishShakeMovement();
}

void AShopUnlockWeaponUIActor::UpdateShakeMovement(float CurveValue)
{
	if (IsValid(WeaponMesh))
	{
		const float PlaybackTime = ShakeTimeline->GetPlaybackPosition();
		const FVector ShakeDirection(
			FMath::PerlinNoise1D(PlaybackTime * 23.0f + 11.3f),
			FMath::PerlinNoise1D(PlaybackTime * 31.0f + 47.1f),
			FMath::PerlinNoise1D(PlaybackTime * 17.0f + 83.7f));

		WeaponMesh->SetRelativeLocation(
			WeaponMeshStartLocation + ShakeDirection * ShakeExtent * CurveValue);
	}
}

void AShopUnlockWeaponUIActor::FinishShakeMovement()
{
	if (IsValid(WeaponMesh))
	{
		WeaponMesh->SetRelativeLocation(WeaponMeshStartLocation);
	}

	if (IsValid(UnlockWeaponEffect))
	{
		UnlockWeaponEffect->Activate(true);
	}

	ResetWeaponIcon();
	OnShakeFinished.Broadcast();
}

void AShopUnlockWeaponUIActor::UpdateHoldShakeMovement(float CurveValue)
{
	if (!bIsHoldShaking || !IsValid(WeaponMesh) || !IsValid(HoldShakeTimeline))
		return;

	const float PlaybackTime = HoldShakeTimeline->GetPlaybackPosition();
	const FVector ShakeDirection(
		FMath::PerlinNoise1D(PlaybackTime * 23.0f + 11.3f),
		FMath::PerlinNoise1D(PlaybackTime * 31.0f + 47.1f),
		FMath::PerlinNoise1D(PlaybackTime * 17.0f + 83.7f));

	WeaponMesh->SetRelativeLocation(
		WeaponMeshStartLocation + ShakeDirection * HoldShakeExtent * CurveValue);
}
