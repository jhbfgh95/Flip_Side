// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ClassSelectButton.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
AClassSelectButton::AClassSelectButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(sphereCollision);
	
	buttonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	buttonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AClassSelectButton::BeginPlay()
{
	Super::BeginPlay();
	coinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	//coinCreateWSubSystem->OffCoinClassSelectMode.AddDynamic(this, &AClassSelectButton::SelectedClass);
}

// Called every frame
void AClassSelectButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClassSelectButton::SelectedClass()
{
	if(coinCreateWSubSystem)
		coinCreateWSubSystem->SetCoinClass(weaponClass);
}

