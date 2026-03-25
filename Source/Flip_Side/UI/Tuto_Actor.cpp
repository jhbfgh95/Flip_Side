// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tuto_Actor.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

// Sets default values
ATuto_Actor::ATuto_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATuto_Actor::BeginPlay()
{
	Super::BeginPlay();
	TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
	if(TutorialSubsystem)
	{
		for(int i =0; i< Order.Num(); i++)
			TutorialSubsystem->SetTutorialEvent(Order[i],this);
	}
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(true);
	
}
void ATuto_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATuto_Actor::ExecuteTutorialEvent_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(false);
}

void ATuto_Actor::FinishTutorialEvent_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(true);
}

