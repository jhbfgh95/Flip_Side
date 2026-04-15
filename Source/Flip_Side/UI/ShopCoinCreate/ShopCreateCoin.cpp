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
		CoinWSubSystem->OnSetWeapon.AddDynamic(this, &AShopCreateCoin::UpdateCoinWeapon);
		//코인 상태 업데이트 됬을때
		//CoinWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &AShopCreateCoin::UpdateCoinWeapon);
		//코인 생성이 클릭 됬을 떄
		//CoinWSubSystem->OnSelectedCoin.AddDynamic(this, &AShopCreateCoin::InitCoin);
	}


	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("RotateCoin"));
	Timeline->AddInterpFloat(CoinRotationCurve, UpdateDelegate);

	FOnTimelineEvent FinishedCoinTrunCallBack;
	FinishedCoinTrunCallBack.BindUFunction(this, FName("FinishedRotateCoin"));
	Timeline->SetTimelineFinishedFunc(FinishedCoinTrunCallBack);


	FrontRotator = CoinMesh->GetRelativeRotation();
}

void AShopCreateCoin::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CoinWSubSystem->OnSetWeapon.RemoveAll(this);
	//CoinWSubSystem->OnSelectedCoinUpdate.RemoveAll(this);
	//CoinWSubSystem->OnSelectedCoin.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
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


//코인 선택됬을 때 초기화
void AShopCreateCoin::InitCoin(FCoinTypeStructure CoinValue, EWeaponClass weponClass)
{

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
	
	CoinMesh->SetRelativeRotation(FrontRotator);
}

void AShopCreateCoin::UpdateCoinWeapon(int32 WeaponID)
{
	//ShopController->SetLockMouse(true);
	bool IsFront = CoinWSubSystem->GetIsCreateCoinFront();
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
	SetCoinSideMatarial();
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


void AShopCreateCoin::ResetSideTexture()
{
	UTexture* Tex = nullptr;
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);
	if(MID)
	{
		MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		MID->SetVectorParameterValue(FName("Back_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}


void AShopCreateCoin::InteractLeftClick_Implementation()
{
	ClickCoin();
}