// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponDescriptionPanelFlap.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/CoinCreateWSubsystem.h"
// Sets default values
AWeaponDescriptionPanelFlap::AWeaponDescriptionPanelFlap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	LeftFlapRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("LeftFlapRootScene"));
	LeftFlapRootScene->SetupAttachment(RootScene);
	
	RightFlapRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RightFlapRootScene"));
	RightFlapRootScene->SetupAttachment(RootScene);

	LeftFlap= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFlap"));
	LeftFlap->SetupAttachment(LeftFlapRootScene);

	RightFlap= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFlap"));
	RightFlap->SetupAttachment(RightFlapRootScene);


	FlapTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FlapTimeline"));

}

// Called when the game starts or when spawned
void AWeaponDescriptionPanelFlap::BeginPlay()
{
	Super::BeginPlay();
	
	CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	CoinCreateSubsystem->OnSelectedCoinUpdate.AddDynamic(this, &AWeaponDescriptionPanelFlap::OpenFlap);

	FOnTimelineFloat FlapCallback;
	FlapCallback.BindUFunction(this, FName("FlapMove"));
	FlapTimeline->AddInterpFloat(FlapCurve, FlapCallback);

	FOnTimelineEvent FinishFlapCallBack;
	FinishFlapCallBack.BindUFunction(this, FName("FinishOpenFlap"));
	FlapTimeline->SetTimelineFinishedFunc(FinishFlapCallBack);


	StartRotatorL = LeftFlapRootScene->GetRelativeRotation();
	ArriveRotatorL = StartRotatorL+ TargetRotator;

	
	StartRotatorR= RightFlapRootScene->GetRelativeRotation();
	ArriveRotatorR= StartRotatorR+ (-1*TargetRotator);
}

void AWeaponDescriptionPanelFlap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CoinCreateSubsystem->OnSelectedCoinUpdate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}


void AWeaponDescriptionPanelFlap::OpenFlap(int Index)
{
	FlapTimeline->PlayFromStart();
}
void AWeaponDescriptionPanelFlap::CloseFlap()
{
	FlapTimeline->ReverseFromEnd();
}

void AWeaponDescriptionPanelFlap::FlapMove(float Value)
{
	FQuat StartQL = StartRotatorL.Quaternion();
    FQuat ArriveQL   = ArriveRotatorL.Quaternion();

	FQuat StartQR = StartRotatorR.Quaternion();
    FQuat ArriveQR   = ArriveRotatorR.Quaternion();

    FQuat MoveQL = FQuat::Slerp(StartQL, ArriveQL, Value);
    FQuat MoveQR = FQuat::Slerp(StartQR, ArriveQR, Value);


    LeftFlapRootScene->SetRelativeRotation(MoveQL);
	RightFlapRootScene->SetRelativeRotation(MoveQR);
}


void AWeaponDescriptionPanelFlap::FinishOpenFlap()
{
	if((0.f<FlapTimeline->GetPlaybackPosition()))
	{
		CloseFlap();
	}
}