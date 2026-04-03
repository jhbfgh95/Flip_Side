// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopSlotCoinCountActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/WidgetComponent.h"
#include "UI/W_CountWidget.h"
// Sets default values
AShopSlotCoinCountActor::AShopSlotCoinCountActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent =RootScene;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(RootScene);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	InteractBox->SetupAttachment(RootScene);

	CoinCountWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CoinCountWidget"));
	CoinCountWidget->SetupAttachment(RootComponent);
	
	CoinTurnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoinTurnTimeline"));
}

// Called when the game starts or when spawned
void AShopSlotCoinCountActor::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

	CoinCountWidgetClass = Cast<UW_CountWidget>(CoinCountWidget->GetUserWidgetObject());

	ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this, &AShopSlotCoinCountActor::SetCountWidget);
	ShopCoinSubsystem->OnCoinCreated.AddDynamic(this, &AShopSlotCoinCountActor::SetCoinWeapon);

	FOnTimelineFloat CoinTurnCallBack;
	CoinTurnCallBack.BindUFunction(this, FName("TurnCoinMovement"));
	CoinTurnTimeline->AddInterpFloat(CoinTurnCurve, CoinTurnCallBack);

	FOnTimelineEvent CoinTurnEndCallBack;
	CoinTurnEndCallBack.BindUFunction(this, FName("EndTurnCoin"));
	CoinTurnTimeline->SetTimelineFinishedFunc(CoinTurnEndCallBack);

	
	StartRotator = CoinMesh->GetRelativeRotation();

	
		CanIteract = true;
}

void AShopSlotCoinCountActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
	ShopCoinSubsystem->OnCoinCreated.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}


void AShopSlotCoinCountActor::InitCoin(int32 SlotIndex)
{
	CoinSlot = SlotIndex;
	SetCoinSide();
	SetCountWidget(CoinSlot, ShopCoinSubsystem->GetSlotCoinCount(CoinSlot));
}


void AShopSlotCoinCountActor::TurnCoinMovement(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);
	CoinMesh->SetRelativeRotation(MoveRotator);
}


void AShopSlotCoinCountActor::EndTurnCoin()
{
	CoinMesh->SetRelativeRotation(ArriveRotator);
	CanIteract = true;
}

void AShopSlotCoinCountActor::ChangeCoinSide()
{
	if(CanIteract)
	{
		CanIteract = false;
		StartRotator = CoinMesh->GetRelativeRotation();
		ArriveRotator = StartRotator + CoinTurnRotator;
		CoinTurnTimeline->PlayFromStart();
	}
}

void AShopSlotCoinCountActor::InteractLeftClick_Implementation()
{
	ChangeCoinSide();
}

void AShopSlotCoinCountActor::SetCountWidget(int32 SlotNum, int32 CoinCount)
{
	if(SlotNum == CoinSlot)
	{
		if(CoinCountWidgetClass)
		{
			CoinCountWidgetClass->SetCountText(ShopCoinSubsystem->GetSlotCoinCount(CoinSlot));
		}
	}
}

void AShopSlotCoinCountActor::SetCoinWeapon(int32 SlotNum, EWeaponClass CoinCount)
{
	if(SlotNum == CoinSlot)
	{
		SetCoinSide();
	}
}

void AShopSlotCoinCountActor::SetCoinSide()
{
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		FCoinTypeStructure CoinData = ShopCoinSubsystem->GetSlotCoin(CoinSlot);

		FFaceData FrontFaceData; 
		FFaceData BackFaceData;

		
		if(DataManagerSubsystem->TryGetWeapon(CoinData.FrontWeaponID, FrontFaceData))
		{
			UE_LOG(LogTemp, Warning, TEXT("앞면 아이콘 설정"));
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData.WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData.TypeColor);
		}
		else
		{
			MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}

		if(DataManagerSubsystem->TryGetWeapon(CoinData.BackWeaponID, BackFaceData))
		{	
			UE_LOG(LogTemp, Warning, TEXT("뒷면아이콘  설정"));
			MID->SetTextureParameterValue(FName("Back_Texture"), BackFaceData.WeaponIcon);
			MID->SetVectorParameterValue(FName("Back_Color"), BackFaceData.TypeColor);
		}
		else
		{
			MID->SetVectorParameterValue(FName("Back_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
	}



}
