// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreateCoinDescriptionPanel.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACreateCoinDescriptionPanel::ACreateCoinDescriptionPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	RootSecen =CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneCompoenent"));

	SetRootComponent(RootSecen);

	ShowPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PannelMeshCompoenent"));
	ReadyPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReadyPanelMeshCompoenent"));

	ShowPanelMesh->SetupAttachment(RootComponent);

	ReadyPanelMesh->SetupAttachment(RootComponent);

	CircularTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CircualrTimeline"));
	LinearTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LinearTimeline"));
}

// Called when the game starts or when spawned
void ACreateCoinDescriptionPanel::BeginPlay()
{
	Super::BeginPlay();

	CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

	if(CoinCreateWSubSystem)
	{
        CoinCreateWSubSystem->OnSelectedCoin.AddDynamic(this, &ACreateCoinDescriptionPanel::InitPannel);
		CoinCreateWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &ACreateCoinDescriptionPanel::GetExplainByID);
	}

	//원운동 타임라인
	FOnTimelineFloat UpdateCircualrCallBack;
	UpdateCircualrCallBack.BindUFunction(this, FName("MoveCircularPanel"));
	CircularTimeline->AddInterpFloat(CircularCurve, UpdateCircualrCallBack);	

	FOnTimelineEvent FinishCircularCallBack;
	FinishCircularCallBack.BindUFunction(this, FName("FinishedMoveCirCular"));
	CircularTimeline->SetTimelineFinishedFunc(FinishCircularCallBack);

	FOnTimelineFloat UpdateLinearCallBack;
	UpdateLinearCallBack.BindUFunction(this, FName("MoveLinearPanel"));
	LinearTimeline->AddInterpFloat(LinearCurve, UpdateLinearCallBack);	

	FOnTimelineEvent FinishLinearCallBack;
	FinishLinearCallBack.BindUFunction(this, FName("FinishedMoveLinear"));
	LinearTimeline->SetTimelineFinishedFunc(FinishLinearCallBack);



	//
	PanelArriveLocation = ShowPanelMesh->GetRelativeLocation() + FVector(100,0,0);
	PanelStartLocation = GetActorLocation();

	ShowPanelLocation = ShowPanelMesh->GetRelativeLocation();
	ReadyPanelLocation = ReadyPanelMesh->GetRelativeLocation();

	Radius = ReadyPanelMesh->GetRelativeLocation().Size();

	ShowToReadyPanelGap =   ShowPanelMesh->GetRelativeLocation()-ReadyPanelMesh->GetRelativeLocation();

	UE_LOG(LogTemp, Warning, TEXT("좌표 %f, %f, %f"), ShowToReadyPanelGap.X, ShowToReadyPanelGap.Y, ShowToReadyPanelGap.Z);

	

	

}

void ACreateCoinDescriptionPanel::InitPannel(FCoinTypeStructure CoinInfo, EWeaponClass CoinClass)
{
	LinearTimeline->PlayFromStart();
}

// Called every frame
void ACreateCoinDescriptionPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACreateCoinDescriptionPanel::MoveLinearPanel(float Value)
{
	FVector MoveVector = FMath::Lerp(ShowPanelLocation, PanelArriveLocation, Value);
	ShowPanelMesh->SetRelativeLocation(MoveVector);
}

void ACreateCoinDescriptionPanel::FinishedMoveLinear()
{
	if (0.f<LinearTimeline->GetPlaybackPosition())
	{
		Radius = ShowPanelMesh->GetRelativeLocation().Size();
		
		CircularTimeline->PlayFromStart();
	}
}

void ACreateCoinDescriptionPanel::MoveCircularPanel(float Value)
{
	float ReadyTheta = FMath::Lerp(PI / 2.f, 0.f, Value);

	float ShowTheta = FMath::Lerp(0.f, -PI / 2.f, Value);

	FVector ReadyMoveLocation;
	
	FVector ShowMoveLocation;
	//1에서 0으로
	ReadyMoveLocation.X = FMath::Cos(ReadyTheta) * Radius;
	//0에서 1로
	ReadyMoveLocation.Y = FMath::Sin(ReadyTheta) * Radius;
	ReadyPanelMesh->SetRelativeLocation(ReadyMoveLocation);

	//1에서 0으로
	ShowMoveLocation.X = FMath::Cos(ShowTheta) * Radius;
	//0에서 1로
	ShowMoveLocation.Y = FMath::Sin(ShowTheta) * Radius;

	ShowPanelMesh->SetRelativeLocation(ShowMoveLocation);
}


void ACreateCoinDescriptionPanel::FinishedMoveCirCular()
{
	ShowPanelMesh->SetRelativeLocation(PanelArriveLocation);
	ReadyPanelMesh->SetRelativeLocation(ReadyPanelLocation);

	LinearTimeline->ReverseFromEnd();
}

void ACreateCoinDescriptionPanel::GetExplainByID(int32 ID)
{
	LinearTimeline->PlayFromStart();
}
