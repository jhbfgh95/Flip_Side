// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/CreateCoinUIActor.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 

#include "Subsystem/DataManagerSubsystem.h" 

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"

#include "Player/ShopController_FlipSide.h"

// Sets default values
ACreateCoinUIActor::ACreateCoinUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	CoinMesh->SetupAttachment(RootComponent);

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PannelTimeline"));

	PressMachineTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PressMachineTimeline"));

	PressMachineMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PressMachineMesh"));
	PressMachineMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACreateCoinUIActor::BeginPlay()
{
	Super::BeginPlay();
    CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	ShopController = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());

	if(CoinCreateWSubSystem)
	{
		//코인클래스 변경됬을때
		CoinCreateWSubSystem->OnCoinClassUpdate.AddDynamic(this, &ACreateCoinUIActor::UpdateWeaponClass);
		//코인 상태 업데이트 됬을때
		CoinCreateWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &ACreateCoinUIActor::UpdateCoinWeapon);
		//코인 생성이 클릭 됬을 떄
		CoinCreateWSubSystem->OnSelectedCoin.AddDynamic(this, &ACreateCoinUIActor::InitCoin);
	}


	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("RotateCoin"));
	Timeline->AddInterpFloat(CoinRotationCurve, UpdateDelegate);

	FOnTimelineEvent FinishedCoinTrunCallBack;
	FinishedCoinTrunCallBack.BindUFunction(this, FName("FinishedRotateCoin"));
	Timeline->SetTimelineFinishedFunc(FinishedCoinTrunCallBack);


	FOnTimelineFloat MachineUpdateDelegate;
	MachineUpdateDelegate.BindUFunction(this, FName("PressCoin"));
	PressMachineTimeline->AddInterpFloat(PressMachineCurve, MachineUpdateDelegate);	
	
	FOnTimelineEvent FinishedPressCoinCallBack;
	FinishedPressCoinCallBack.BindUFunction(this, FName("FinishedPressCoin"));
	PressMachineTimeline->SetTimelineFinishedFunc(FinishedPressCoinCallBack);

	FOnTimelineEvent EventFunc;
    EventFunc.BindUFunction(this, FName("SetCoinSideMatarial"));
    PressMachineTimeline->AddEvent(0.1f, EventFunc); 

	MachineStartLocation = PressMachineMesh->GetRelativeLocation();

}

void ACreateCoinUIActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CoinCreateWSubSystem->OnCoinClassUpdate.RemoveAll(this);
	CoinCreateWSubSystem->OnSelectedCoinUpdate.RemoveAll(this);
	CoinCreateWSubSystem->OnSelectedCoin.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ACreateCoinUIActor::ClickCoin()
{
	ShopController->SetLockMouse(true);

	StartRotator = CoinMesh->GetRelativeRotation();
	ArriveRotator = StartRotator + CoinTurnRotator;

	//앞뒤 변경
	if(CoinCreateWSubSystem->GetIsCreateCoinFront())
	{
		Timeline->PlayFromStart();
	}
	else
	{
		Timeline->Reverse();
	}

	CoinCreateWSubSystem->ChangeCoinSide();
}

void ACreateCoinUIActor::PressCoin(float Value)
{
	FVector MoveVector = FMath::Lerp(MachineStartLocation, MachineEndLocation, Value);
	PressMachineMesh->SetRelativeLocation(MoveVector);
}

void ACreateCoinUIActor::FinishedPressCoin()
{
	ShopController->SetLockMouse(false);
}

void ACreateCoinUIActor::RotateCoin(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);
	CoinMesh->SetRelativeRotation(MoveRotator);
}

void ACreateCoinUIActor::FinishedRotateCoin()
{
	CoinMesh->SetRelativeRotation(ArriveRotator);
	ShopController->SetLockMouse(false);
}

void ACreateCoinUIActor::UpdateWeaponClass(EWeaponClass weponClass)
{
	WeaponType = weponClass;
}

//코인 선택됬을 때 초기화
void ACreateCoinUIActor::InitCoin(FCoinTypeStructure CoinValue, EWeaponClass weponClass)
{
	CoinInfo = CoinValue;
	UpdateWeaponClass(weponClass);

	//1.ID에 맞는 값을 할당함
	if(!DataManager->TryGetWeapon(CoinValue.FrontWeaponID, FrontFaceData))
	{
		FrontFaceData.WeaponID = -1;
	}
	if(!DataManager->TryGetWeapon(CoinValue.BackWeaponID, BackFaceData))
	{
		BackFaceData.WeaponID = -1;
	}

	if(weponClass == EWeaponClass::None)
	{
		ResetSideTexture();
	}
	else
	{
		SetCoinSideMatarial();
	}
}

void ACreateCoinUIActor::UpdateCoinWeapon(int32 WeaponID, bool IsFront)
{
	ShopController->SetLockMouse(true);
	PressMachineTimeline->PlayFromStart();

	if(IsFront)
	{
		if(!DataManager->TryGetWeapon(WeaponID, FrontFaceData))
		{
			FrontFaceData.WeaponID = -1;
		}
	}
	else
	{
		if(!DataManager->TryGetWeapon(WeaponID, BackFaceData))
		{
			BackFaceData.WeaponID = -1;
		}
	}
}


void ACreateCoinUIActor::SetCoinSideMatarial()
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


void ACreateCoinUIActor::ResetSideTexture()
{
	UTexture* Tex = nullptr;
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);
	if(MID)
	{
		MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		MID->SetVectorParameterValue(FName("Back_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}


void ACreateCoinUIActor::InteractLeftClick_Implementation()
{
	ClickCoin();
}