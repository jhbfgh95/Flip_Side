// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemSellPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
// Sets default values
AShopItemSellPanel::AShopItemSellPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	PanelAnchorComponent= CreateDefaultSubobject<USceneComponent>(TEXT("AnchorScene"));
	PanelAnchorComponent->SetupAttachment(RootScene);

	ItemSellPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SellPanel"));
	ItemSellPanel->SetupAttachment(PanelAnchorComponent);

	

	PanelMoveTimeline= CreateDefaultSubobject<UTimelineComponent>(TEXT("PanelMoveTimeline"));
}

// Called when the game starts or when spawned
void AShopItemSellPanel::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat PanelMoveCallback;
	PanelMoveCallback.BindUFunction(this, FName("PanelMove"));
	PanelMoveTimeline->AddInterpFloat(PanelMoveCurve, PanelMoveCallback);

	/*
	FOnTimelineEvent FinishFlapCallBack;
	FinishFlapCallBack.BindUFunction(this, FName("FinishOpenFlap"));
	PanelMoveTimeline->SetTimelineFinishedFunc(FinishFlapCallBack);
	*/

	
	StartRotator = RootScene->GetRelativeRotation();
	ArriveRotator = StartRotator+ TargetRotator;
}

void AShopItemSellPanel::PanelMove(float Value)
{
	FQuat StartQ = StartRotator.Quaternion();
    FQuat ArriveQ   = ArriveRotator.Quaternion();

    FQuat MoveQ = FQuat::Slerp(StartQ, ArriveQ, Value);

    PanelAnchorComponent->SetRelativeRotation(MoveQ);
}

void AShopItemSellPanel::OpenPanel()
{
	PanelMoveTimeline->PlayFromStart();
}

void AShopItemSellPanel::ClosePanel()
{
	PanelMoveTimeline->ReverseFromEnd();
}
