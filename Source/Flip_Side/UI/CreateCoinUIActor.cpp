// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreateCoinUIActor.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACreateCoinUIActor::ACreateCoinUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(sphereCollision);
	
	coinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	coinMesh->SetupAttachment(RootComponent);

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PannelTimeline"));
}

// Called when the game starts or when spawned
void ACreateCoinUIActor::BeginPlay()
{
	Super::BeginPlay();
    CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("RotateCoin"));
	Timeline->AddInterpFloat(CoinRotationCurve, UpdateDelegate);	

	StartRotation = GetActorRotation();

}

// Called every frame
void ACreateCoinUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACreateCoinUIActor::ClickCoin()
{
	if(IsCoinFront)
		IsCoinFront = false;
	else
		IsCoinFront = true;

	Timeline->PlayFromStart();
	CoinCreateWSubSystem->ChangeCoinSide();
}

void ACreateCoinUIActor::RotateCoin(float Value)
{
	SetActorRotation(StartRotation + FRotator(0.f,Value,0.f));
}



void ACreateCoinUIActor::UpdateCoinWeapon(int32 WeaponID)
{
	if(IsCoinFront)
	{
		CoinInfo.FrontWeaponID = WeaponID;
	}
	else
	{
		CoinInfo.BackWeaponID = WeaponID;
	}
}

void ACreateCoinUIActor::SetCoinSideMatarial()
{

}