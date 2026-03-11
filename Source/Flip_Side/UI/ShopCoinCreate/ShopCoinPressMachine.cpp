// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/ShopCoinPressMachine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
// Sets default values
AShopCoinPressMachine::AShopCoinPressMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;
	
	PressMachineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PresssMachineMesh"));
	PressMachineMesh->SetupAttachment(RootComponent);
	
	PressMachineTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PressMacineTimeline"));
}

// Called when the game starts or when spawned
void AShopCoinPressMachine::BeginPlay()
{
	Super::BeginPlay();
	
}



void AShopCoinPressMachine::PressCoin(float Value)
{
	
}