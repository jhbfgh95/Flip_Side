// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreateCoinUIActor.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Subsystem/ShopWeaponDataWSubsystem.h" 
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
// Sets default values
ACreateCoinUIActor::ACreateCoinUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	
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
	WeaponDataSubSystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();

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

	FOnTimelineFloat MachineUpdateDelegate;
	MachineUpdateDelegate.BindUFunction(this, FName("PressCoin"));
	PressMachineTimeline->AddInterpFloat(PressMachineCurve, MachineUpdateDelegate);	

	FOnTimelineEvent EventFunc;
    EventFunc.BindUFunction(this, FName("SetCoinSideMatarial"));
    PressMachineTimeline->AddEvent(0.1f, EventFunc); 

	StartRotation = GetActorRotation();

	MachineStartLocation = PressMachineMesh->GetComponentLocation();

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
	//앞뒤 변경
	if(IsCoinFront)
		IsCoinFront = false;
	else
		IsCoinFront = true;

	Timeline->PlayFromStart();
	CoinCreateWSubSystem->ChangeCoinSide();
}

void ACreateCoinUIActor::PressCoin(float Value)
{
	FVector MoveVector = FMath::Lerp(MachineStartLocation, MachineEndLocation, Value);
	PressMachineMesh->SetRelativeLocation(MoveVector);
}

void ACreateCoinUIActor::RotateCoin(float Value)
{
	SetActorRotation(StartRotation + FRotator(0.f,Value,0.f));
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
	
	FrontFaceData = nullptr;
	BackFaceData = nullptr;
	FrontWeaponIndex = -1;
	BackWeaponIndex = -1;
	IsCoinFront = true;

	//최적화를 위해 ID를 무기배열의 Index로 변경
	FrontWeaponIndex = CoinInfo.FrontWeaponID;// 
	BackWeaponIndex = CoinInfo.BackWeaponID;// WeaponDataSubSystem->GetWeaponIndexByID(weponClass, CoinInfo.BackWeaponID);

	if(weponClass == EWeaponClass::None)
	{
		ResetSideTexture();
	}
	else
	{
		SetCoinSideWeaponData(FrontFaceData, FrontWeaponIndex);
		SetCoinSideWeaponData(BackFaceData, BackWeaponIndex);	
		SetCoinSideMatarial();
	}
}

void ACreateCoinUIActor::UpdateCoinWeapon(int32 WeaponIndex)
{
	PressMachineTimeline->PlayFromStart();

	if(IsCoinFront)
	{
		//FrontWeaponIndex = WeaponIndex;
		//CoinInfo.FrontWeaponID = WeaponDataSubSystem->GetWeaponDataByIndex(WeaponType, WeaponIndex)->WeaponID;
		SetCoinSideWeaponData(FrontFaceData, WeaponIndex);
	}
	else
	{
		//CoinInfo.BackWeaponID = WeaponIndex;
		//CoinInfo.BackWeaponID = WeaponDataSubSystem->GetWeaponDataByIndex(WeaponType, WeaponIndex)->WeaponID;
		SetCoinSideWeaponData(BackFaceData, WeaponIndex);
	}
}


void ACreateCoinUIActor::SetCoinSideWeaponData(const FFaceData*& FaceData , int32 Index)
{
	if(Index == -1)
	{
		FaceData = nullptr;
		return;
	}
	//인덱스를 기반으로 무기 데이터 설정
	FaceData = WeaponDataSubSystem->GetWeaponDataByIndex(WeaponType, Index);
}

void ACreateCoinUIActor::SetCoinSideMatarial()
{
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		if(FrontFaceData)
		{
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData->WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData->TypeColor);

		}
		if(BackFaceData)
		{	
			MID->SetTextureParameterValue(FName("Back_Texture"), BackFaceData->WeaponIcon);
			MID->SetVectorParameterValue(FName("Back_Color"), BackFaceData->TypeColor);
		}
	
		if(!FrontFaceData&&!BackFaceData)
		{
			ResetSideTexture();
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

