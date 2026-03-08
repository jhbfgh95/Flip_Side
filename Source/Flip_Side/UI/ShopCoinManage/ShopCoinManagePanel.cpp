// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinManagePanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Components/WidgetComponent.h"
#include "DataTypes/CoinDataTypes.h"

#include "WeaponDataTypes.h"
#include "Subsystem/ShopLevel/ShopWeaponDataWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"


#include "UI/ShopCoinManage/W_CoinManagePanelWidget.h"
#include "UI/ShopCoinManage/CoinSlotLockPanel.h"
#include "UI/ShopCoinManage/ShopCoinManageCoin.h"
#include "UI/ShopCoinManage/ShopCoinSlotCountButton.h"

// Sets default values
AShopCoinManagePanel::AShopCoinManagePanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
    //스태틱 메쉬들
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Panel"));
    PanelMesh->SetupAttachment(RootScene);

	GearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GearMesh"));
	GearMesh->SetupAttachment(RootScene);
	
	DescriptionMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DescriptionMesh"));
	DescriptionMesh->SetupAttachment(RootScene);
	
	IncreaseCountButton = CreateDefaultSubobject<UChildActorComponent>(TEXT("IncreaseCountButton"));
	IncreaseCountButton->SetupAttachment(RootScene);

	DecreaseCountButton= CreateDefaultSubobject<UChildActorComponent>(TEXT("DecreaseCountButton"));
	DecreaseCountButton->SetupAttachment(RootScene);

	CoinActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("CoinActor"));
	CoinActor->SetupAttachment(RootScene);

	LockPanel= CreateDefaultSubobject<UChildActorComponent>(TEXT("LockPanel"));
	LockPanel->SetupAttachment(RootScene);




	//x타임라인들//

	GearTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("GearlTimeline"));

	DescriptionTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("PanelDownTimeLine"));

	//위젯들
	PanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PanelWidget"));
	PanelWidget->SetupAttachment(RootScene);

	DescribeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DescribeWidget"));
	DescribeWidget->SetupAttachment(DescriptionMesh);

	

}

// Called when the game starts or when spawned
void AShopCoinManagePanel::BeginPlay()
{
	Super::BeginPlay();

	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopWeaponDataSubsystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();
	DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AShopCoinManagePanel::ActiveGear);
	ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this, &AShopCoinManagePanel::SetPanelWidget);
	
	

	//Set Actor Class
	IncreaseCountButtonClass = Cast<AShopCoinSlotCountButton>(IncreaseCountButton->GetChildActor());;
	DecreaseCountButtonClass = Cast<AShopCoinSlotCountButton>(DecreaseCountButton->GetChildActor());
	LockPanelClass= Cast<ACoinSlotLockPanel>(LockPanel->GetChildActor());
	CoinActorClass= Cast<AShopCoinManageCoin>(CoinActor->GetChildActor());
	PanelWidgetClass = Cast<UW_CoinManagePanelWidget>(PanelWidget->GetUserWidgetObject());


	//설명 판넬 운동 타임라인
	FOnTimelineFloat DescriptionPanelMoveCallBack;
	DescriptionPanelMoveCallBack.BindUFunction(this, FName("MoveDescriptionPanel"));
	DescriptionTimeLine->AddInterpFloat(DescriptionPanelCurve, DescriptionPanelMoveCallBack);

	FOnTimelineFloat GearMoveCallBack;
	GearMoveCallBack.BindUFunction(this, FName("RotateGear"));
	GearTimeline->AddInterpFloat(GearCurve, GearMoveCallBack);


	DescriptionPanelStartVec = DescriptionMesh->GetRelativeLocation();
	DescriptionPanelArriveVec = DescriptionPanelStartVec + DescriptionPanelMoveDirection;


	StartGearRotator = GearMesh->GetRelativeRotation();
	ArriveGearRotator = StartGearRotator + GearRotateDirection;

}

void AShopCoinManagePanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopCoinSubsystem->OnCoinSlotChange.RemoveAll(this);
	ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}
void AShopCoinManagePanel::ActiveDescriptionPanel(bool IsPanelShow)
{
	if(IsPanelShow)
		DescriptionTimeLine->PlayFromStart();
	else
		DescriptionTimeLine->ReverseFromEnd();
}

void AShopCoinManagePanel::MoveDescriptionPanel(float Value)
{
	FVector MoveVec = FMath::Lerp(DescriptionPanelStartVec, DescriptionPanelArriveVec,Value);
	DescriptionMesh->SetRelativeLocation(MoveVec);
}


void AShopCoinManagePanel::RotateGear(float Value)
{
	FQuat GearStartQ = StartGearRotator.Quaternion();

	FQuat GearArriveQ = ArriveGearRotator.Quaternion();

    FQuat MoveQ = FQuat::Slerp(GearStartQ, GearArriveQ, Value);

	GearMesh->SetWorldRotation(MoveQ);
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
	DescriptionMesh->SetRelativeLocation(DescriptionPanelStartVec);
	LockPanelClass->InitLockPanel(!ShopCoinSubsystem->GetCurrentCoinUnlock());
	CoinActorClass->InitCoin();
}

void AShopCoinManagePanel::SetPanelWidget(int32 CoinSlotIndex, int32 CoinCount)
{
	PanelWidgetClass->SetCountText(CoinCount);
}
void AShopCoinManagePanel::InitPanelAfterArrive()
{
	if(ShopCoinSubsystem->GetCurrentCoinUnlock())
		ActiveDescriptionPanel(true);
}


void AShopCoinManagePanel::OpenDescriptionPanel()
{
	DescriptionTimeLine->PlayFromStart();
}

void AShopCoinManagePanel::InitPanelToStart()
{
	if(0 < DescriptionTimeLine->GetPlaybackPosition())
		ActiveDescriptionPanel(false);
}


void AShopCoinManagePanel::InitPanelCoin()
{
	CoinActorClass->InitCoin();
}
