// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/ShopCreateCoin.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h" 
#include "Subsystem/DataManagerSubsystem.h" 

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"

#include "Player/ShopController_FlipSide.h"
// Sets default values
AShopCreateCoin::AShopCreateCoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	CoinMesh->SetupAttachment(RootComponent);

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PannelTimeline"));
}

// Called when the game starts or when spawned
void AShopCreateCoin::BeginPlay()
{
	Super::BeginPlay();
    CoinWSubSystem =  GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	ShopController = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());

	if(CoinWSubSystem)
	{
		//코인클래스 변경됬을때
		CoinWSubSystem->OnSetWeapon.AddDynamic(this, &AShopCreateCoin::SetWeaponAdaptor);
		//코인 상태 업데이트 됬을때
		CoinWSubSystem->OnCoinSlotChange.AddDynamic(this, &AShopCreateCoin::SetCoin);
	}


	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("RotateCoin"));
	Timeline->AddInterpFloat(CoinRotationCurve, UpdateDelegate);

	FOnTimelineEvent FinishedCoinTrunCallBack;
	FinishedCoinTrunCallBack.BindUFunction(this, FName("FinishedRotateCoin"));
	Timeline->SetTimelineFinishedFunc(FinishedCoinTrunCallBack);


	FrontRotator = CoinMesh->GetRelativeRotation();

	SetCoin();
}

void AShopCreateCoin::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CoinWSubSystem->OnSetWeapon.RemoveAll(this);
	CoinWSubSystem->OnCoinSlotChange.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void AShopCreateCoin::SetCoin()
{
	/*
	if(!CoinWSubSystem->GetIsCreateCoinFront())
	{
		ClickCoin();
	}*/
	SetCoinWeaponID();
	SetCoinSideMatarial();
}

void AShopCreateCoin::ClickCoin()
{
	ShopController->SetLockMouse(true);

	StartRotator = CoinMesh->GetRelativeRotation();
	ArriveRotator = StartRotator + CoinTurnRotator;

	//앞뒤 변경
	if(CoinWSubSystem->GetIsCreateCoinFront())
	{
		Timeline->PlayFromStart();
	}
	else
	{
		Timeline->Reverse();
	}

	CoinWSubSystem->ChangeCoinSide();
}

void AShopCreateCoin::RotateCoin(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);
	CoinMesh->SetRelativeRotation(MoveRotator);
}

void AShopCreateCoin::FinishedRotateCoin()
{
	CoinMesh->SetRelativeRotation(ArriveRotator);
	ShopController->SetLockMouse(false);
}


void AShopCreateCoin::SetCoinWeaponID()
{
	//1.ID에 맞는 값을 할당함
	if(!DataManager->TryGetWeapon(CoinWSubSystem->GetCurrentCoinWeaponID(true), FrontFaceData))
	{
		FrontFaceData.WeaponID = -1;
	}
	if(!DataManager->TryGetWeapon(CoinWSubSystem->GetCurrentCoinWeaponID(false), BackFaceData))
	{
		BackFaceData.WeaponID = -1;
	}
	UE_LOG(LogTemp,Warning, TEXT("앞면 무기 아디%d"), FrontFaceData.WeaponID);
	UE_LOG(LogTemp,Warning, TEXT("뒷면 무기 아디%d"), BackFaceData.WeaponID);
}

void AShopCreateCoin::SetCoinSideMatarial()
{
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		if(FrontFaceData.WeaponID != -1)
		{
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData.WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData.TypeColor);

		}
		else
		{
			MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
		if(BackFaceData.WeaponID != -1)
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


void AShopCreateCoin::InteractLeftClick_Implementation()
{
	ClickCoin();
}

void AShopCreateCoin::SetWeaponAdaptor(int32 WeaponID)
{
	SetCoin();
}