// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManagePanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Components/WidgetComponent.h"
#include "DataTypes/CoinDataTypes.h"

#include "WeaponDataTypes.h"
#include "Subsystem/ShopWeaponDataWSubsystem.h"
// Sets default values
AShopCoinManagePanel::AShopCoinManagePanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
    //스태틱 메쉬들
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Panel"));
    PanelMesh->SetupAttachment(RootScene);

	GearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GearMesh"));
	GearMesh->SetupAttachment(RootScene);
	LockPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockPanel"));
    LockPanelMesh->SetupAttachment(RootScene);
	
	DescriptionMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DescriptionMesh"));
	DescriptionMesh->SetupAttachment(RootScene);

	PanelCoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelCoinMesh"));
	PanelCoinMesh->SetupAttachment(RootScene);

	//x타임라인들//
	UnlockTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("UnlockPanelTimeLine"));

	GearTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("GearlTimeline"));

	PanelCoinTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PanelCoinTimeline"));
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

	ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AShopCoinManagePanel::ActiveGear);
	//ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &AShopCoinManagePanel::UnlockPanel);
	

	//잠김 판넬 운동 타임라인
	FOnTimelineFloat LockPanelMoveCallBack;
	LockPanelMoveCallBack.BindUFunction(this, FName("LockPanelMove"));
	UnlockTimeLine->AddInterpFloat(UnlockPanelCurve, LockPanelMoveCallBack);	

	//설명 판넬 운동 타임라인
	FOnTimelineFloat DescriptionPanelMoveCallBack;
	DescriptionPanelMoveCallBack.BindUFunction(this, FName("MoveDescriptionPanel"));
	DescriptionTimeLine->AddInterpFloat(DescriptionPanelCurve, DescriptionPanelMoveCallBack);

	FOnTimelineFloat GearMoveCallBack;
	GearMoveCallBack.BindUFunction(this, FName("RotateGear"));
	GearTimeline->AddInterpFloat(GearCurve, GearMoveCallBack);


	LockPanelStartVec = LockPanelMesh->GetRelativeLocation();
	LockPanelArriveVec = LockPanelStartVec+ LockPanelMoveDirection;

	DescriptionPanelStartVec = DescriptionMesh->GetRelativeLocation();
	DescriptionPanelArriveVec = DescriptionPanelStartVec + DescriptionPanelMoveDirection;
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
void AShopCoinManagePanel::ActiveDescriptionPanel(bool IsPanelShow)
{
	if(IsPanelShow)
		DescriptionTimeLine->PlayFromStart();
	else
		DescriptionTimeLine->ReverseFromEnd();
}

void AShopCoinManagePanel::UnlockPanel()
{
	UnlockTimeLine->PlayFromStart();
	DescriptionTimeLine->PlayFromStart();
}

void AShopCoinManagePanel::LockPanelMove(float Value)
{
	FVector MoveVec = FMath::Lerp(LockPanelStartVec, LockPanelArriveVec,Value);
	LockPanelMesh->SetRelativeLocation(MoveVec);
}

void AShopCoinManagePanel::MoveDescriptionPanel(float Value)
{
	FVector MoveVec = FMath::Lerp(DescriptionPanelStartVec, DescriptionPanelArriveVec,Value);
	DescriptionMesh->SetRelativeLocation(MoveVec);
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

	DescriptionMesh->SetRelativeLocation(DescriptionPanelStartVec);
	InitPanelCoin();
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
	
	UMaterialInstanceDynamic* MID = PanelCoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		FCoinTypeStructure CoinData = ShopCoinSubsystem->GetCurrentSlotCoin();
		EWeaponClass CurrentWeaponClass = ShopCoinSubsystem->GetCurrentSlotCoinClass();
		const FFaceData* FrontFaceData = ShopWeaponDataSubsystem->GetWeaponDataByIndex(CurrentWeaponClass, CoinData.FrontWeaponID);	
		const FFaceData* BackFaceData = ShopWeaponDataSubsystem->GetWeaponDataByIndex(CurrentWeaponClass, CoinData.BackWeaponID);
		UE_LOG(LogTemp, Warning, TEXT("코인 아이콘 설정 / 앞면 아이디 %d"), CoinData.FrontWeaponID);
		UE_LOG(LogTemp, Warning, TEXT("코인 아이콘 설정 / 뒷면 아이디 %d"), CoinData.BackWeaponID);
		if(FrontFaceData)
		{
			UE_LOG(LogTemp, Warning, TEXT("앞면 아이콘 설정"));
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData->WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData->TypeColor);

		}
		if(BackFaceData)
		{	
			UE_LOG(LogTemp, Warning, TEXT("뒷면아이콘  설정"));
			MID->SetTextureParameterValue(FName("Back_Texture"), BackFaceData->WeaponIcon);
			MID->SetVectorParameterValue(FName("Back_Color"), BackFaceData->TypeColor);
		}
	
		if(CoinData.FrontWeaponID == -1)
		{
			MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
		if(CoinData.BackWeaponID == -1)
		{
			MID->SetVectorParameterValue(FName("Back_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
	}
}
