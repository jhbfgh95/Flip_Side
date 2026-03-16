// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/SelectUnlockWeaponUIActor.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/ShopLevel/ShopWeaponDataWSubsystem.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "Player/ShopController_FlipSide.h"
// Sets default values
ASelectUnlockWeaponUIActor::ASelectUnlockWeaponUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	LockCoinScene= CreateDefaultSubobject<USceneComponent>(TEXT("LockCoinScene"));
	LockCoinScene->SetupAttachment(RootComponent);

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMeshComponent"));
	CoinMesh->SetupAttachment(LockCoinScene);
	LockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LoceshComponent"));
	LockMesh->SetupAttachment(LockCoinScene);

	JitterTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("JitterTimeline"));
	RiseTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RiseTimeline"));
	SizeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SizeTimeline"));
}

// Called when the game starts or when spawned
void ASelectUnlockWeaponUIActor::BeginPlay()
{
	Super::BeginPlay();
	ShopUnlockWeaponSubSystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
	WeaponDataSubsystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();
	UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
	ShopContoller = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());

	ShopUnlockWeaponSubSystem->OnSelectUnlockWeapon.AddDynamic(this , &ASelectUnlockWeaponUIActor::SetUnlockCoin);
	UnlockSubsystem->OnWeaponUnlock.AddDynamic(this, &ASelectUnlockWeaponUIActor::UnlockWeapon);

	FOnTimelineFloat RiseCallBack;
	RiseCallBack.BindUFunction(this, FName("RiseCoin"));
	RiseTimeline->AddInterpFloat(RiseCurve, RiseCallBack);

	FOnTimelineEvent FinishedRiseCallBack;
	FinishedRiseCallBack.BindUFunction(this, FName("FinishedRiseCoin"));
	RiseTimeline->SetTimelineFinishedFunc(FinishedRiseCallBack);

	FOnTimelineFloat JitterCallBack;
	JitterCallBack.BindUFunction(this, FName("JitterCoin"));
	JitterTimeline->AddInterpFloat(JitterCurve, JitterCallBack);

	FOnTimelineEvent FinishedJitterCallBack;
	FinishedJitterCallBack.BindUFunction(this, FName("FinishedJitterCoin"));
	JitterTimeline->SetTimelineFinishedFunc(FinishedJitterCallBack);

	FOnTimelineFloat SizeCallBack;
	SizeCallBack.BindUFunction(this, FName("ChangeSizeCoin"));
	SizeTimeline->AddInterpFloat(SizeCurve, SizeCallBack);

	FOnTimelineEvent FinishedSizeCallBack;
	FinishedSizeCallBack.BindUFunction(this, FName("FinishedChangeSizeCoin"));
	SizeTimeline->SetTimelineFinishedFunc(FinishedSizeCallBack);

	/////////////////////////////////////////////

	StartLocation = LockCoinScene->GetRelativeLocation();
	ArriveLocation = StartLocation + RiseDirection;

	StartSize = CoinMesh->GetRelativeScale3D();

	////////////////
	LockMesh->SetVisibility(false);
}

void ASelectUnlockWeaponUIActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopUnlockWeaponSubSystem->OnSelectUnlockWeapon.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}


void ASelectUnlockWeaponUIActor::SetUnlockCoin(EWeaponClass WeaponClass, int32 index, bool IsWeaponUnlock)
{
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		const FFaceData* FrontFaceData = WeaponDataSubsystem->GetWeaponDataByIndex(ShopUnlockWeaponSubSystem->GetCurrentWeaponClass()
		, ShopUnlockWeaponSubSystem->GetCurrentUnlockWeaponIndex());
		
		if(FrontFaceData)
		{
			UE_LOG(LogTemp, Warning, TEXT("앞면 아이콘 설정"));
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData->WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData->TypeColor);

		}

		if(ShopUnlockWeaponSubSystem->GetCurrentUnlockWeaponIndex() == -1)
		{
			MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
	}

	if(!IsWeaponUnlock)
	{
		LockMesh->SetVisibility(true);
	}
	else
	{
		LockMesh->SetVisibility(false);
	}

}

void ASelectUnlockWeaponUIActor::UnlockWeapon(EWeaponClass WeaponClass, int32 AddIndex)
{
	ShopContoller->SetLockMouse(true);
	RiseTimeline->PlayFromStart();
}

void ASelectUnlockWeaponUIActor::JitterCoin(float Value)
{
	float X = FMath::FRandRange(-JitterValue, JitterValue);
	float Y = FMath::FRandRange(-JitterValue, JitterValue);
	FVector Offset = FVector(X, Y, 0) + ArriveLocation;

	FVector JitterVec = FMath::Lerp(ArriveLocation, Offset, Value);
	
	LockCoinScene->SetRelativeLocation(JitterVec);
}

void ASelectUnlockWeaponUIActor::RiseCoin(float Value)
{
	FVector RiseVec = FMath::Lerp(StartLocation, ArriveLocation, Value);
	LockCoinScene->SetRelativeLocation(RiseVec);
}

void ASelectUnlockWeaponUIActor::ChangeSizeCoin(float Value)
{
	FVector ScaleVec = FMath::Lerp(StartSize, MaxCoinSize, Value);
	CoinMesh->SetRelativeScale3D(ScaleVec);
}


void ASelectUnlockWeaponUIActor::FinishedRiseCoin()
{
	if(0<RiseTimeline->GetPlaybackPosition())
	{
		JitterTimeline->PlayFromStart();
	}
	else
	{
		FinishedUnlockCoinAnim();
	}
}
	
void ASelectUnlockWeaponUIActor::FinishedJitterCoin()
{
	SizeTimeline->PlayFromStart();
}

void ASelectUnlockWeaponUIActor::FinishedChangeSizeCoin()
{
	LockMesh->SetVisibility(false);
	RiseTimeline->ReverseFromEnd();
}

void ASelectUnlockWeaponUIActor::FinishedUnlockCoinAnim()
{
	
	ShopContoller->SetLockMouse(false);
}