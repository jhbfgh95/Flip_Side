// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/CoinSlotIndexButtonLock.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
// Sets default values
ACoinSlotIndexButtonLock::ACoinSlotIndexButtonLock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootScene =CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	LockPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockPanelMesh"));
	LockPanelMesh->SetupAttachment(RootComponent);
	
	LockImageMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockImageMesh"));
	LockImageMesh->SetupAttachment(LockPanelMesh);

	LockPanelTimeline= CreateDefaultSubobject<UTimelineComponent>(TEXT("LockPanelTimeline"));
}

// Called when the game starts or when spawned
void ACoinSlotIndexButtonLock::BeginPlay()
{
	Super::BeginPlay();
	
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &ACoinSlotIndexButtonLock::UnlockSlotPanel);

	FOnTimelineFloat PanelMoveCallBack;
	PanelMoveCallBack.BindUFunction(this, FName("PanelMovement"));
	LockPanelTimeline->AddInterpFloat(LockPanelCurve, PanelMoveCallBack);

	PanelStartVector = LockPanelMesh->GetRelativeLocation();
	PanelArriveVector = PanelStartVector + PanelMoveDirection;

}

void ACoinSlotIndexButtonLock::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopCoinSubsystem->OnUnlockCoinSlot.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ACoinSlotIndexButtonLock::PanelMovement(float Value)
{
	FVector TargetVec = FMath::Lerp(PanelStartVector, PanelArriveVector, Value);
	LockPanelMesh->SetRelativeLocation(TargetVec);
}
	
void ACoinSlotIndexButtonLock::UnlockSlotPanel()
{
	if(!IsOpen && ShopCoinSubsystem->GetCoinUnlockByIndex(SlotIndexNum))
	{
		IsOpen = true;
		LockImageMesh->SetHiddenInGame(true);
		LockPanelTimeline->PlayFromStart();
	}
}