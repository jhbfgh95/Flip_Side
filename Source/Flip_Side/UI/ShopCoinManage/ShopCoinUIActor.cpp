// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinUIActor.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AShopCoinUIActor::AShopCoinUIActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CoinRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CoinRoot"));
	RootComponent = CoinRoot;

	FrontMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontMesh"));
	FrontMesh->SetupAttachment(CoinRoot);

	BackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackMesh"));
	BackMesh->SetupAttachment(CoinRoot);

	SwapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SwapCollision"));
	SwapCollision->SetupAttachment(CoinRoot);
	SwapCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwapCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SwapCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SwapCollision->ComponentTags.Add(TEXT("LClickAble"));

	SwapTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SwapTimeline"));
}

void AShopCoinUIActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(SwapTimeline) && IsValid(SwapCurve))
	{
		FOnTimelineFloat UpdateCallback;
		UpdateCallback.BindUFunction(this, FName("UpdateSwapMovement"));
		SwapTimeline->AddInterpFloat(SwapCurve, UpdateCallback);

		FOnTimelineEvent FinishedCallback;
		FinishedCallback.BindUFunction(this, FName("FinishSwapMovement"));
		SwapTimeline->SetTimelineFinishedFunc(FinishedCallback);
	}

	SetFrontCoin(FrontFaceData);
	SetBackCoin(BackFaceData);
}

void AShopCoinUIActor::SetFrontCoin(const FFaceData& InFrontFaceData)
{
	if(InFrontFaceData.WeaponID ==-1)
	{
		ResetFrontWeaponIcon();
		return;
	}

	FrontFaceData = InFrontFaceData;

	if (!IsValid(FrontMesh))
	{
		return;
	}

	if (!IsValid(FrontMaterialInstance))
	{
		FrontMaterialInstance = FrontMesh->CreateDynamicMaterialInstance(0);
	}

	if (IsValid(FrontMaterialInstance))
	{
		FrontMaterialInstance->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData.WeaponIcon);
		FrontMaterialInstance->SetVectorParameterValue(FName("Front_Color"), TypeColor);
	}
}

void AShopCoinUIActor::SetBackCoin(const FFaceData& InBackFaceData)
{
	if(InBackFaceData.WeaponID ==-1)
	{
		ResetBackWeaponIcon();
		return;
	}

	BackFaceData = InBackFaceData;

	if (!IsValid(BackMesh))
	{
		return;
	}

	if (!IsValid(BackMaterialInstance))
	{
		BackMaterialInstance = BackMesh->CreateDynamicMaterialInstance(0);
	}

	if (IsValid(BackMaterialInstance))
	{
		BackMaterialInstance->SetTextureParameterValue(FName("Back_Texture"), BackFaceData.WeaponIcon);
		BackMaterialInstance->SetVectorParameterValue(FName("Back_Color"), TypeColor);
	}
}

void AShopCoinUIActor::ResetWeaponIcons()
{
	ResetFrontWeaponIcon();
	ResetBackWeaponIcon();
}

void AShopCoinUIActor::ResetFrontWeaponIcon()
{
	FrontFaceData = FFaceData();

	if (IsValid(FrontMaterialInstance))
	{
		FrontMaterialInstance->SetTextureParameterValue(FName("Front_Texture"), nullptr);
		FrontMaterialInstance->SetVectorParameterValue(FName("Front_Color"), FLinearColor::Black);
	}
}

void AShopCoinUIActor::ResetBackWeaponIcon()
{
	BackFaceData = FFaceData();
	if (IsValid(BackMaterialInstance))
	{
		BackMaterialInstance->SetTextureParameterValue(FName("Back_Texture"), nullptr);
		BackMaterialInstance->SetVectorParameterValue(FName("Back_Color"), FLinearColor::Black);
	}
}

bool AShopCoinUIActor::SwapMeshPositions()
{
	if (!IsValid(SwapTimeline) || !IsValid(SwapCurve) || SwapTimeline->IsPlaying() || !IsValid(FrontMesh) || !IsValid(BackMesh))
	{
		return false;
	}

	FrontStartLocation = FrontMesh->GetRelativeLocation();
	BackStartLocation = BackMesh->GetRelativeLocation();
	SwapTimeline->PlayFromStart();
	return true;
}

void AShopCoinUIActor::InteractLeftClick_Implementation()
{
	if (SwapMeshPositions())
	{
		OnShopCoinSideChanged.Broadcast();
	}
}

void AShopCoinUIActor::UpdateSwapMovement(float Alpha)
{
	if (!IsValid(FrontMesh) || !IsValid(BackMesh))
	{
		return;
	}

	const float ArcOffset = FMath::Sin(Alpha * PI) * SwapArcHeight;
	FrontMesh->SetRelativeLocation(FMath::Lerp(FrontStartLocation, BackStartLocation, Alpha) + FVector::RightVector * ArcOffset);
	BackMesh->SetRelativeLocation(FMath::Lerp(BackStartLocation, FrontStartLocation, Alpha) - FVector::RightVector * ArcOffset);
}
void AShopCoinUIActor::FinishSwapMovement()
{
	if (IsValid(FrontMesh) && IsValid(BackMesh))
	{
		FrontMesh->SetRelativeLocation(BackStartLocation);
		BackMesh->SetRelativeLocation(FrontStartLocation);
	}
}
