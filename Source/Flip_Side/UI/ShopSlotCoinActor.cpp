// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopSlotCoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
AShopSlotCoinActor::AShopSlotCoinActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMeshComponent"));
	SetRootComponent(CoinMesh);
	CoinMoveTimeline= CreateDefaultSubobject<UTimelineComponent>(TEXT("CoinMoveTimeline"));
}

// Called when the game starts or when spawned
void AShopSlotCoinActor::BeginPlay()
{
	Super::BeginPlay();
	
	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("MoveCoin"));
	CoinMoveTimeline->AddInterpFloat(CoinMoveCurve, UpdateDelegate);	

	
	FOnTimelineEvent FinishDelegate;
	FinishDelegate.BindUFunction(this, FName("FinishCoinTimeline"));
	CoinMoveTimeline->SetTimelineFinishedFunc(FinishDelegate);
	StartLocation = GetActorLocation();

	//Actor->SetActorHiddenInGame(true);
}

// Called every frame
void AShopSlotCoinActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//코인 넣음
void AShopSlotCoinActor::MoveCoin(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, EndLocation, Value);
	SetActorLocation(MoveVector);
}

//코인 뻄
/*
void AShopSlotCoinActor::RemoveCoin(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, EndLocation, Value);
	SetActorLocation(MoveVector);
}*/

void AShopSlotCoinActor::FinishCoinTimeline()
{
	if(!bISCoinInSlot)
	{
		SetActorHiddenInGame(true);
	}
}

void AShopSlotCoinActor::AddCoin()
{
	SetActorHiddenInGame(true);
	bISCoinInSlot = true;
	CoinMoveTimeline->PlayFromStart();
}

void AShopSlotCoinActor::RemoveCoin()
{
	bISCoinInSlot = false;
	CoinMoveTimeline->ReverseFromEnd();
}