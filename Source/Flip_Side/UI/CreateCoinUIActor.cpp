// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreateCoinUIActor.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
// Sets default values
ACreateCoinUIActor::ACreateCoinUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

	if(CoinCreateWSubSystem)
	{
		//코인클래스 변경됬을때
		CoinCreateWSubSystem->OnCoinClassUpdate.AddDynamic(this, &ACreateCoinUIActor::UpdateWeaponClass);
		//코인 상태 업데이트 됬을때
		CoinCreateWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &ACreateCoinUIActor::UpdateCoinWeapon);
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

// Called every frame
void ACreateCoinUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACreateCoinUIActor::ClickCoin()
{
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
	FrontFaceData = nullptr;
	BackFaceData = nullptr;
	IsCoinFront = true;
	UpdateWeaponClass(weponClass);

	if(weponClass == EWeaponClass::None)
	{
		ResetSideTexture();
	}
	else
	{
		SetCoinSide(FrontFaceData, CoinInfo.FrontWeaponID);

		SetCoinSide(BackFaceData, CoinInfo.BackWeaponID);	

		SetCoinSideMatarial();
	}
}

void ACreateCoinUIActor::UpdateCoinWeapon(int32 WeaponID)
{
	PressMachineTimeline->PlayFromStart();

	if(IsCoinFront)
	{
		CoinInfo.FrontWeaponID = WeaponID;
		SetCoinSide(FrontFaceData, WeaponID);
	}
	else
	{
		CoinInfo.BackWeaponID = WeaponID;
		SetCoinSide(BackFaceData, WeaponID);
	}


}


void ACreateCoinUIActor::SetCoinSide(const FFaceData*& FaceData , int32 ID)
{
	if(ID == -1)
		FaceData = nullptr;
	

	if(WeaponType == EWeaponClass::Tank)
	{
		FaceData = CoinCreateWSubSystem->GetTankWeaponData(ID);
	}
	else if(WeaponType == EWeaponClass::Deal)
	{	
		FaceData = CoinCreateWSubSystem->GetDealWeaponData(ID);
	}
	else if(WeaponType == EWeaponClass::Heal)
	{
		FaceData = CoinCreateWSubSystem->GetUtilWeaponData(ID);
	}
	else
	{
		FaceData = nullptr;
	}

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
	
		if(FrontFaceData&&BackFaceData)
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

