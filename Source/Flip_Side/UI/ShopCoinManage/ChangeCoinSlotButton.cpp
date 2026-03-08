// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ChangeCoinSlotButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

// Sets default values
AChangeCoinSlotButton::AChangeCoinSlotButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent =RootScene;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(RootScene);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	InteractBox->SetupAttachment(RootScene);

	ButtonPressTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ButtonPressTimeline"));

}

// Called when the game starts or when spawned
void AChangeCoinSlotButton::BeginPlay()
{
	Super::BeginPlay();
	
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

	StartLocation = ButtonMesh->GetRelativeLocation();
	ArriveLocation = StartLocation + PressDirection;

	FOnTimelineFloat PressButtonCallBack;
	PressButtonCallBack.BindUFunction(this, FName("MoveLockPanel"));
	ButtonPressTimeline->AddInterpFloat(PressCurve, PressButtonCallBack);

}

void AChangeCoinSlotButton::InteractLeftClick_Implementation()
{
	if(IsIncrease)
		ShopCoinSubsystem->ChangeCoinSlotRight();
	else
		ShopCoinSubsystem->ChangeCoinSlotLeft();

	ButtonPressTimeline->PlayFromStart();
}

void AChangeCoinSlotButton::MoveButton(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, ArriveLocation, Value);
	ButtonMesh->SetRelativeLocation(MoveVector); 
}