// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "DataTypes/GridTypes.h"
#include "DataTypes/WeaponDataTypes.h"

ACoinActor::ACoinActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CoinRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = CoinRootComp;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coin Mesh"));
	CoinMesh->SetupAttachment(RootComponent);

	CoinHPUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Coin HP UI"));
	CoinHPUI->SetupAttachment(RootComponent);

	/* 처음 GridPoint는 없는거 (-1) */
	CurrentGridPoint.GridX = -1;
	CurrentGridPoint.GirdY = -1;
}

void ACoinActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoinActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ACoinActor::GetCoinID()
{
	return CoinID;
}

EFaceState ACoinActor::GetCoinFace()
{
	return CurrentFace;
}

void ACoinActor::SetCoinFace(EFaceStae DecidedFace)
{
	if(DecidedFace == EFaceState::None) return;

	CurrentFace = DecidedFace;
	
	if(CurrentFace == EFaceState::Front)
	{
		DecidedWeaponID = FrontWeaponID;
	}
	else if(CurrentFace == EFaceState::Back)
	{
		DecidedWeaponID = BackWeaponID;
	}
}

//BattleManager에서 SetGridPoint 부를 때 X, Y 최대값을 GridManager에서 받아서 그거 넘어가면 Return하고 랜덤값 다시 만드는 코드 있어야함!! 
void ACoinActor::SetGridPoint(FGridPoint DecidedGridPoint)
{
	CurrentGridPoint.GridX = DecidedGridPoint.GridX;
	CurrentGridPoint.GridY = DecidedGridPoint.GridY;
}