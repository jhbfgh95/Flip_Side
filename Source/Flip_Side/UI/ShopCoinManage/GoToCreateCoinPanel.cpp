// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/GoToCreateCoinPanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimeLineComponent.h"
#include "Components/BoxComponent.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"
#include "Player/GameMode_Shop.h"
#include "Player/ShopController_FlipSide.h"
// Sets default values
AGoToCreateCoinPanel::AGoToCreateCoinPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootScene= CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);

	// 몸통 메쉬
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Panel"));
    PanelMesh->SetupAttachment(RootScene);
	//잠긴 판넬 표시 메쉬
	LockPanelMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockPanelMesh"));
    LockPanelMesh->SetupAttachment(RootScene);

	ButtonMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(RootScene);

	ItneractBox= CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
    ItneractBox->SetupAttachment(RootScene);

	LockPanelTimeLine  = CreateDefaultSubobject<UTimelineComponent>(TEXT("LockPanelTimeLine"));
	ButtonTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ButtonTimeline"));
}

// Called when the game starts or when spawned
void AGoToCreateCoinPanel::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopCoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	ShopGameMode =  Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
	ShopController = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());
	
	//판넬 운동 타임라인
	FOnTimelineFloat LockPanelTimeLineCallBack;
	LockPanelTimeLineCallBack.BindUFunction(this, FName("MoveLockPanel"));
	LockPanelTimeLine->AddInterpFloat(LockPanelCurve, LockPanelTimeLineCallBack);	

	FOnTimelineEvent FinishLockPanelCallBack;
	FinishLockPanelCallBack.BindUFunction(this, FName("FinishMoveLockPanel"));
	LockPanelTimeLine->SetTimelineFinishedFunc(FinishLockPanelCallBack);

	FOnTimelineFloat PressButtonTimeLineCallBack;
	PressButtonTimeLineCallBack.BindUFunction(this, FName("MoveButton"));
	ButtonTimeline->AddInterpFloat(ButtonCurve, PressButtonTimeLineCallBack);
	
	FOnTimelineEvent FinishPressButtonCallBack;
	FinishPressButtonCallBack.BindUFunction(this, FName("FinishedMoveButton"));
	ButtonTimeline->SetTimelineFinishedFunc(FinishPressButtonCallBack);

	PressStartVector = ButtonMesh->GetRelativeLocation();
	PressArriveVector = PressStartVector + PressTargetVector;

	StartVector = LockPanelMesh->GetRelativeLocation();
	ArriveVector = StartVector + TargetVector;


	ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AGoToCreateCoinPanel::InitLockPanel);
	ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &AGoToCreateCoinPanel::OpenlockPanel);
}

void AGoToCreateCoinPanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopCoinSubsystem->OnCoinSlotChange.RemoveAll(this);
	ShopCoinSubsystem->OnUnlockCoinSlot.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void AGoToCreateCoinPanel::InitLockPanel(bool IsChangetoBottom)
{
	if(ShopCoinSubsystem->GetCurrentCoinUnlock())
	{
		if(!IsLockPanelOpen)
		{
			LockPanelTimeLine->PlayFromStart();
		}
	}
	else
	{
		if(IsLockPanelOpen)
		{
			LockPanelTimeLine->ReverseFromEnd();
		}
	}
}

void AGoToCreateCoinPanel::OpenlockPanel()
{
	LockPanelTimeLine->PlayFromStart();
}

void AGoToCreateCoinPanel::MoveLockPanel(float Value)
{
	FVector MoveValue = FMath::Lerp(StartVector, ArriveVector, Value);
	LockPanelMesh->SetRelativeLocation(MoveValue);
}

void AGoToCreateCoinPanel::MoveButton(float Value)
{
	FVector MoveValue = FMath::Lerp(PressStartVector, PressArriveVector, Value);
	ButtonMesh->SetRelativeLocation(MoveValue);
	UE_LOG(LogTemp,Warning, TEXT("%f"), Value);
}

void AGoToCreateCoinPanel::FinishedMoveButton()
{
	ShopController->SetLockMouse(false);
	ShopCoinCreateSubsystem->SelectCoin( ShopCoinSubsystem->GetCurrentSlotCoin(),ShopCoinSubsystem->GetCurrentSlotCoinClass());
	ShopGameMode->SetCoinCreateMode();
}

void AGoToCreateCoinPanel::ChangeCreateCoinMode()
{
	ShopController->SetLockMouse(true);
	ButtonTimeline->PlayFromStart();
}

void AGoToCreateCoinPanel::FinishMoveLockPanel()
{
	if (0.f<LockPanelTimeLine->GetPlaybackPosition())
	{
		IsLockPanelOpen = true;
	}
	else
	{
		IsLockPanelOpen = false;
	}
}


void AGoToCreateCoinPanel::InteractLeftClick_Implementation()
{
	ChangeCreateCoinMode();
}