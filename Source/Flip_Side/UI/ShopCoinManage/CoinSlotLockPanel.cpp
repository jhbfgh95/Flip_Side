// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/CoinSlotLockPanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
// Sets default values
ACoinSlotLockPanel::ACoinSlotLockPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent =RootScene;

	LockPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockPanelMesh"));
	LockPanelMesh->SetupAttachment(RootScene);

	PanelInteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	PanelInteractBox->SetupAttachment(RootScene);

	PanelMoveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LockPanelMoveTimeline"));
}

// Called when the game starts or when spawned
void ACoinSlotLockPanel::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

	FOnTimelineFloat UnlockPanelCallBack;
	UnlockPanelCallBack.BindUFunction(this, FName("MoveLockPanel"));
	PanelMoveTimeline->AddInterpFloat(PanelMoveCurve, UnlockPanelCallBack);
}

void ACoinSlotLockPanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ACoinSlotLockPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoinSlotLockPanel::InitLockPanel(FVector SetLocation)
{
	SetActorLocation(SetLocation);
}

void ACoinSlotLockPanel::MoveLockPanel(float Value)
{
	FVector MoveVecor = FMath::Lerp(PanelStartLocation, PanelArriveLocation, Value);
	SetActorLocation(MoveVecor);
}

void ACoinSlotLockPanel::UnlockCoinSlot()
{
	if(!ShopCoinSubsystem->GetCurrentCoinUnlock())
	{
		PanelStartLocation = GetActorLocation();
		PanelArriveLocation = PanelStartLocation + PanelMoveDirection;
		ShopCoinSubsystem->UnlockCurrentCoinSlot();
		PanelMoveTimeline->PlayFromStart();
	}
}

void ACoinSlotLockPanel::InteractLeftClick_Implementation()
{
	UnlockCoinSlot();
}	
	
FVector ACoinSlotLockPanel::GetUnlockPanelDirection()
{
	return PanelMoveDirection;
}