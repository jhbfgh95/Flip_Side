// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GoToCreateCoinPanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimeLineComponent.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Player/GameMode_Shop.h"
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

	LockPanelTimeLine  = CreateDefaultSubobject<UTimelineComponent>(TEXT("LockPanelTimeLine"));
}

// Called when the game starts or when spawned
void AGoToCreateCoinPanel::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopGameMode =  Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

	//판넬 운동 타임라인
	FOnTimelineFloat LockPanelTimeLineCallBack;
	LockPanelTimeLineCallBack.BindUFunction(this, FName("MoveLockPanel"));
	LockPanelTimeLine->AddInterpFloat(LockPanelCurve, LockPanelTimeLineCallBack);	

	FOnTimelineEvent FinishLockPanelCallBack;
	FinishLockPanelCallBack.BindUFunction(this, FName("FinishMoveLockPanel"));
	LockPanelTimeLine->SetTimelineFinishedFunc(FinishLockPanelCallBack);

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

void AGoToCreateCoinPanel::ChangeCreateCoinMode()
{
	ShopGameMode->SetCoinCreateMode();
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