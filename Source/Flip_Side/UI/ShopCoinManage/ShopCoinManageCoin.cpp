// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinManageCoin.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Player/ShopController_FlipSide.h"
// Sets default values
AShopCoinManageCoin::AShopCoinManageCoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent =RootScene;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(RootScene);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	InteractBox->SetupAttachment(RootScene);

	CoinTurnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoinTurnTimeline"));
}

// Called when the game starts or when spawned
void AShopCoinManageCoin::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	ShopController = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());

	FOnTimelineFloat CoinTurnCallBack;
	CoinTurnCallBack.BindUFunction(this, FName("TurnCoinMovement"));
	CoinTurnTimeline->AddInterpFloat(CoinTurnCurve, CoinTurnCallBack);

	FOnTimelineEvent FinishedCoinTrunCallBack;
	FinishedCoinTrunCallBack.BindUFunction(this, FName("FinishedTurnCoin"));
	CoinTurnTimeline->SetTimelineFinishedFunc(FinishedCoinTrunCallBack);
	InitRotator = CoinMesh->GetRelativeRotation();
	StartRotator = CoinMesh->GetRelativeRotation();
}



void AShopCoinManageCoin::TurnCoinMovement(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);
	CoinMesh->SetRelativeRotation(MoveRotator);
}

void AShopCoinManageCoin::FinishedTurnCoin()
{
	CoinMesh->SetRelativeRotation(ArriveRotator);
	ShopController->SetLockMouse(false);
}

void AShopCoinManageCoin::ChangeCoinSide()
{
	ShopController->SetLockMouse(true);
	StartRotator = CoinMesh->GetRelativeRotation();
	ArriveRotator = StartRotator + CoinTurnRotator;

	if(IsFront)
	{
		IsFront = false;
	}
	else
	{
		IsFront = true;
	}
	
	ShopCoinSubsystem->ChangeSlotCoinSide(IsFront);
	
	CoinTurnTimeline->PlayFromStart();
	
}

void AShopCoinManageCoin::InteractLeftClick_Implementation()
{
	ChangeCoinSide();
}	
void AShopCoinManageCoin::InitCoin()
{
	IsFront = true;
	CoinMesh->SetRelativeRotation(InitRotator);

	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		FCoinTypeStructure CoinData = ShopCoinSubsystem->GetCurrentSlotCoin();
		EWeaponClass CurrentWeaponClass = ShopCoinSubsystem->GetCurrentSlotCoinClass();

		FFaceData FrontFaceData; 
		FFaceData BackFaceData;

		
		if(DataManagerSubsystem->TryGetWeapon(CoinData.FrontWeaponID, FrontFaceData))
		{
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData.WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData.TypeColor);
		}
		else
		{
			MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}

		if(DataManagerSubsystem->TryGetWeapon(CoinData.BackWeaponID, BackFaceData))
		{	
			MID->SetTextureParameterValue(FName("Back_Texture"), BackFaceData.WeaponIcon);
			MID->SetVectorParameterValue(FName("Back_Color"), BackFaceData.TypeColor);
		}
		else
		{
			MID->SetVectorParameterValue(FName("Back_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
	}
}
