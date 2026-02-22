// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManagePanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Components/WidgetComponent.h"
// Sets default values
AShopCoinManagePanel::AShopCoinManagePanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
    
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Panel"));
    PanelMesh->SetupAttachment(RootScene);

	GearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GearMesh"));
	GearMesh->SetupAttachment(PanelMesh);
	LockPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockPanel"));
    LockPanelMesh->SetupAttachment(PanelMesh);
	
	DescritionMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DescritionMesh"));
	DescritionMesh->SetupAttachment(PanelMesh);

	UnlockTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("UnlockPanelTimeLine"));

	GearTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("GearlTimeline"));

	PanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PanelWidget"));
	PanelWidget->SetupAttachment(PanelMesh);

	DescribeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DescribeWidget"));
	DescribeWidget->SetupAttachment(DescritionMesh);

	DescriptionTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("PanelDownTimeLine"));

}

// Called when the game starts or when spawned
void AShopCoinManagePanel::BeginPlay()
{
	Super::BeginPlay();

	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AShopCoinManagePanel::ActiveGear);
	ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &AShopCoinManagePanel::UnlockPanel);


	//잠김 판넬 운동 타임라인
	FOnTimelineFloat LockPanelMoveCallBack;
	LockPanelMoveCallBack.BindUFunction(this, FName("LockPanelMove"));
	UnlockTimeLine->AddInterpFloat(PanelRotateCurve, LockPanelMoveCallBack);	

	//설명 판넬 운동 타임라인
	FOnTimelineFloat DescriptionPanelMoveCallBack;
	DescriptionPanelMoveCallBack.BindUFunction(this, FName("MoveDescriptionPanel"));
	DescriptionTimeLine->AddInterpFloat(PanelRotateCurve, DescriptionPanelMoveCallBack);

	FOnTimelineFloat GearMoveCallBack;
	GearMoveCallBack.BindUFunction(this, FName("RotateGear"));
	GearTimeline->AddInterpFloat(GearCurve, GearMoveCallBack);


	LockPanelStartVec = LockPanelMesh->GetRelativeLocation();
	LockPanelArriveVec = LockPanelStartVec+ LockPanelMoveDirection;
/*
	//판넬 운동 끝났을 때 타임라인
	FOnTimelineEvent FinishLockPanelMoveCallBack;
	FinishLockPanelMoveCallBack.BindUFunction(this, FName("FinishedPanelMove"));
	UnlockTimeLine->SetTimelineFinishedFunc(FinishLockPanelMoveCallBack);*/

}

// Called every frame
void AShopCoinManagePanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShopCoinManagePanel::UnlockPanel()
{
	UnlockTimeLine->PlayFromStart();
}

void AShopCoinManagePanel::LockPanelMove(float Value)
{
	FVector MoveVec = FMath::Lerp(LockPanelStartVec, LockPanelArriveVec,Value);
	LockPanelMesh->SetRelativeLocation(MoveVec);
}

void AShopCoinManagePanel::MoveDescriptionPanel(float Value)
{
	FVector MoveVec = FMath::Lerp(DescriptionPanelStartVec, DescriptionPanelArriveVec,Value);
	DescritionMesh->SetRelativeLocation(MoveVec);
}


void AShopCoinManagePanel::RotateGear(float Value)
{
	float Pitch = Value * GearRotateAngle;
    FRotator GearRotator = FRotator(0,0,0) + FRotator(0,0, Pitch);

	GearMesh->SetRelativeRotation(GearRotator);
}


void AShopCoinManagePanel::ActiveGear(bool IsPanelMoveToBottom)
{
	if(IsPanelMoveToBottom)
		GearTimeline->ReverseFromEnd();
	else
		GearTimeline->PlayFromStart();
}


void AShopCoinManagePanel::InitPanel()
{
	if(!ShopCoinSubsystem->GetCurrentCoinUnlock())
		LockPanelMesh->SetRelativeLocation(LockPanelStartVec);
	else
		LockPanelMesh->SetRelativeLocation(LockPanelArriveVec);
}