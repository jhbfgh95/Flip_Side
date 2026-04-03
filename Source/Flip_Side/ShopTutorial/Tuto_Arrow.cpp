// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_Arrow.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

// Sets default values
ATuto_Arrow::ATuto_Arrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATuto_Arrow::BeginPlay()
{
	Super::BeginPlay();
	TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
	if(TutorialSubsystem)
		TutorialSubsystem->SetTutorialEvent(Order,this);
	
	SetActorHiddenInGame(true);
}

// Called every frame
void ATuto_Arrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATuto_Arrow::ExecuteTutorialEvent_Implementation()
{
	SetActorHiddenInGame(false);
}

void ATuto_Arrow::FinishTutorialEvent_Implementation()
{
	SetActorHiddenInGame(true);
}


void ATuto_Arrow::CallEvent()
{
	TutorialSubsystem->ExecuteEvents(0);
}