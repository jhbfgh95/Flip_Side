// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinManagePanelGear.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
// Sets default values
AShopCoinManagePanelGear::AShopCoinManagePanelGear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = false;
	
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
	
	GearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GearMesh"));
	GearMesh->SetupAttachment(RootScene);

	GearTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("GearlTimeline"));

}

// Called when the game starts or when spawned
void AShopCoinManagePanelGear::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	//ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AShopCoinManagePanelGear::ActiveGear);

	FOnTimelineFloat GearMoveCallBack;
	GearMoveCallBack.BindUFunction(this, FName("RotateGear"));
	GearTimeline->AddInterpFloat(GearCurve, GearMoveCallBack);
	

	StartGearRotator = GearMesh->GetRelativeRotation();
}

void AShopCoinManagePanelGear::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopCoinSubsystem->OnCoinSlotChange.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}


void AShopCoinManagePanelGear::RotateGear(float Value)
{
    FRotator MoveRotator = FMath::Lerp(StartGearRotator, ArriveGearRotator, Value);
	if(IsGearRotateUp)
		MoveRotator*=-1;
	GearMesh->SetRelativeRotation(MoveRotator);
}

void AShopCoinManagePanelGear::ActiveGear(bool IsPanelMoveToBottom)
{
	StartGearRotator = GearMesh->GetRelativeRotation();
	ArriveGearRotator = StartGearRotator + GearRotateDirection;
	IsGearRotateUp = IsPanelMoveToBottom;
	ArriveGearRotator = StartGearRotator + GearRotateDirection;
	GearTimeline->PlayFromStart();
}