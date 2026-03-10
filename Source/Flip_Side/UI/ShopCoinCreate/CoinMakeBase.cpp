// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/CoinMakeBase.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
// Sets default values
ACoinMakeBase::ACoinMakeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootSecen =CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneCompoenent"));
	SetRootComponent(RootSecen);

	CoinBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinBaseMeshCompoenent"));

	CoinBaseMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACoinMakeBase::BeginPlay()
{
	Super::BeginPlay();
	CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	if(CoinCreateSubsystem)
	{
		CoinCreateSubsystem->OnCoinClassUpdate.AddDynamic(this, &ACoinMakeBase::SetMaterial);
	}
}

void ACoinMakeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	CoinCreateSubsystem->OnCoinClassUpdate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ACoinMakeBase::SetMaterial(EWeaponClass WeaponClass)
{
	UMaterialInstanceDynamic* MID = CoinBaseMesh->CreateDynamicMaterialInstance(1);
	if(MID)
	{
		switch(WeaponClass)
		{
			case EWeaponClass::Tank:
				MID->SetVectorParameterValue(FName("TypeColor"), TankClassColor);
				break;
			case EWeaponClass::Deal:
				MID->SetVectorParameterValue(FName("TypeColor"), DealClassColor);
				break;
			case EWeaponClass::Heal:
				MID->SetVectorParameterValue(FName("TypeColor"), UtilClassColor);
				break;
			default:
				MID->SetVectorParameterValue(FName("TypeColor"), DefaultColor);
				break;
		}
		
	}
}

