// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ClassButtonPannel.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 

// Sets default values
AClassButtonPannel::AClassButtonPannel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PannelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PannelMeshCompoenent"));
	SetRootComponent(PannelMesh);
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PannelTimeline"));
}

// Called when the game starts or when spawned
void AClassButtonPannel::BeginPlay()
{
	Super::BeginPlay();
	/*
	StartLocation = GetActorLocation();
	EndLocation = StartLocation + FVector(0,10,0);*/
    coinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();


	coinCreateWSubSystem->OnCoinClassSelectMode.AddDynamic(this, &AClassButtonPannel::WorkPannel);
	coinCreateWSubSystem->OffCoinClassSelectMode.AddDynamic(this, &AClassButtonPannel::WorkPannel);
	
	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("MovePannel"));
	Timeline->AddInterpFloat(PannelMoveCurve, UpdateDelegate);	


	FOnTimelineEvent FinishDelegate;
	FinishDelegate.BindUFunction(this, FName("FinishMovePannel"));
	Timeline->SetTimelineFinishedFunc(FinishDelegate);
	bIsPannelOpen = false;

	StartLocation = GetActorLocation();
	EndLocation = StartLocation +FVector(0,0,500.f);

}

// Called every frame
void AClassButtonPannel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClassButtonPannel::MovePannel(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, EndLocation, Value);
	SetActorLocation(MoveVector);
}


void AClassButtonPannel::FinishMovePannel()
{
	if(bIsPannelOpen)
		bIsPannelOpen = false;
	else
		bIsPannelOpen = true;
}
void AClassButtonPannel::WorkPannel()
{
	if(!bIsPannelOpen) //&& GetActorLocation().Equals(StartLocation, 1.0f))
	{
		OpenPannel();
	}
	else
	{
		ClosePannel();
	}
}

void AClassButtonPannel::OpenPannel()
{
	Timeline->PlayFromStart();
}


void AClassButtonPannel::ClosePannel()
{
	Timeline->ReverseFromEnd();
}
