// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponDescriptionPanelFlap.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/CoinCreateWSubsystem.h"
// Sets default values
AWeaponDescriptionPanelFlap::AWeaponDescriptionPanelFlap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	LeftFlap= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFlap"));
	LeftFlap->SetupAttachment(RootScene);

	RightFlap= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFlap"));
	RightFlap->SetupAttachment(RootScene);


	FlapTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FlapTimeline"));

}

// Called when the game starts or when spawned
void AWeaponDescriptionPanelFlap::BeginPlay()
{
	Super::BeginPlay();
	
	CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	//CoinCreateSubsystem->OnWeapo.AddDynamic(this, AWeaponDescriptionPanelFlap::OpenFlap);

	FOnTimelineFloat FlapCallback;
	FlapCallback.BindUFunction(this, FName("FlapMove"));
	FlapTimeline->AddInterpFloat(FlapCurve, FlapCallback);

	FOnTimelineEvent FinishFlapCallBack;
	FinishFlapCallBack.BindUFunction(this, FName("FinishOpenFlap"));
	FlapTimeline->SetTimelineFinishedFunc(FinishFlapCallBack);


	StartRotatorL = LeftFlap->GetRelativeRotation();
	ArriveRotatorL = StartRotatorL+ TargetRotator;

	
	StartRotatorR= RightFlap->GetRelativeRotation();
	ArriveRotatorR= StartRotatorR+ (-1*TargetRotator);
}

// Called every frame
void AWeaponDescriptionPanelFlap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AWeaponDescriptionPanelFlap::OpenFlap()
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


    LeftFlap->SetRelativeRotation(MoveQL);
	RightFlap->SetRelativeRotation(MoveQR);
}


void AWeaponDescriptionPanelFlap::FinishOpenFlap()
{
	if(!FlapTimeline->IsReversing())
	{
		CloseFlap();
	}
}