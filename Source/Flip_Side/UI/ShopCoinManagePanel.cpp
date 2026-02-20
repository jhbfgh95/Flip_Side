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
	
	DescribeMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DescribeMesh"));
	DescribeMesh->SetupAttachment(PanelMesh);

	PanelTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("PanelTimeLine"));
	UnlockTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("UnlockPanelTimeLine"));
	
	PanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PanelWidget"));
	PanelWidget->SetupAttachment(PanelMesh);

	DescribeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DescribeWidget"));
	DescribeWidget->SetupAttachment(DescribeMesh);

	PaneDownlTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("PanelDownTimeLine"));

}

// Called when the game starts or when spawned
void AShopCoinManagePanel::BeginPlay()
{
	Super::BeginPlay();

	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

	ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AShopCoinManagePanel::ChangeSlot);
	//판넬 운동 타임라인
	FOnTimelineFloat PanelTimeLineCallBack;
	PanelTimeLineCallBack.BindUFunction(this, FName("RotatePanel"));
	PanelTimeLine->AddInterpFloat(PanelRotateCurve, PanelTimeLineCallBack);	

	//판넬 운동 끝났을 때 타임라인
	FOnTimelineEvent FinishPanelTimeLineCallBack;
	FinishPanelTimeLineCallBack.BindUFunction(this, FName("FinishedPanelMove"));
	PanelTimeLine->SetTimelineFinishedFunc(FinishPanelTimeLineCallBack);

	
	PanelStartVector = PanelMesh->GetRelativeLocation();
	PanelTopArriveVector = PanelStartVector + PanelMoveTopDirect; 
	PanelBottomArriveVector = PanelStartVector - PanelMoveTopDirect; 

	StartVector = LockPanelMesh->GetRelativeLocation();
	ArriveVector = StartVector + TargetVector;
}


// Called every frame
void AShopCoinManagePanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AShopCoinManagePanel::ChangeSlot()
{
	PanelTimeLine->PlayFromStart();
}

void AShopCoinManagePanel::InitPanel()
{
	if(ShopCoinSubsystem->GetCurrentCoinUnlock())
	{
		UnlockPanel();
	}
	else
	{

	}
}

void AShopCoinManagePanel::RunPanel(bool IsInCrease)
{

}



void AShopCoinManagePanel::MovePanelUp(float Value)
{
	FVector MoveVector = FMath::Lerp(PanelStartVector,PanelTopArriveVector,Value);
	LockPanelMesh->SetRelativeLocation(MoveVector);
}

void AShopCoinManagePanel::RotatePanel(float Value)
{
	//루트 씬 컴포넌트가 회전되도록
	//FRotator MoveRotate = FMath::Lerp(StartRotate,TargetRotate,Value);
	//RootScene->SetWorldRotation(MoveRotate);
}

void AShopCoinManagePanel::FinishedPanelMove()
{
	
}	
void AShopCoinManagePanel::MovePanelDown(float Value)
{
	
}

void AShopCoinManagePanel::UnlockPanel()
{
	ShopCoinSubsystem->UnlockCoin();
	UnlockTimeLine->PlayFromStart();
}

void AShopCoinManagePanel::LockPanelMove(float Value)
{
	FVector MoveVector = FMath::Lerp(StartVector,ArriveVector,Value);
	LockPanelMesh->SetRelativeLocation(MoveVector);
}