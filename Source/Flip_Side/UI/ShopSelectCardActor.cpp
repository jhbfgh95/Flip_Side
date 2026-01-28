// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopSelectCardActor.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/WidgetComponent.h"

// Sets default values
AShopSelectCardActor::AShopSelectCardActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMeshCompoenent"));
	SetRootComponent(CardMesh);
	//Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PannelTimeline"));

	CardWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ExplainWidget"));
	CardWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AShopSelectCardActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShopSelectCardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

