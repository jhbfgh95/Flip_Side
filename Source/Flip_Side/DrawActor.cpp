// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawActor.h"

ADrawActor::ADrawActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DrawRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = DrawRootComp;

	DrawMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Draw Mesh"));
	DrawMesh->SetupAttachment(RootComponent);
}

void ADrawActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADrawActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

