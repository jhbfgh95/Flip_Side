// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ExplainPannel.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 

// Sets default values
AExplainPannel::AExplainPannel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PannelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PannelMeshCompoenent"));
	SetRootComponent(PannelMesh);
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PannelTimeline"));

}

// Called when the game starts or when spawned
void AExplainPannel::BeginPlay()
{
	Super::BeginPlay();
	
	CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

	if(CoinCreateWSubSystem)
	{
		CoinCreateWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &AExplainPannel::UpdateExplanationText);
	}


	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("RotatePannel"));
	Timeline->AddInterpFloat(PannelRotationCurve, UpdateDelegate);	


	FOnTimelineEvent FinishDelegate;
	FinishDelegate.BindUFunction(this, FName("FinishRotationPannel"));
	Timeline->SetTimelineFinishedFunc(FinishDelegate);


}

// Called every frame
void AExplainPannel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplainPannel::WorkPannel()
{
	Timeline->PlayFromStart();
}

void AExplainPannel::FinishRotationPannel()
{
}

void AExplainPannel::RotatePannel(float Value)
{
	SetActorRotation(ShowRotation + FRotator(Value,0.f,  0.f));
}

void AExplainPannel::UpdateExplanationText(int32 WeaponID)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), WeaponID);
	WorkPannel();
}