// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinSlotIndexButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "UI/ShopCoinManage/W_ShopCoinSlotIndex.h"
#include "UI/ShopCoinManage/ShopSlotCoinCountActor.h"
// Sets default values
AShopCoinSlotIndexButton::AShopCoinSlotIndexButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootScene =CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(RootComponent);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
	InteractBox->SetupAttachment(RootComponent);

	CoinCountActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("CoinCountActor"));
	CoinCountActor->SetupAttachment(RootComponent);

	SlotIndexWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("IndexWidget"));
	SlotIndexWidget->SetupAttachment(RootComponent);
	
	ButtonTimeline= CreateDefaultSubobject<UTimelineComponent>(TEXT("ButtonTimeline"));
}

// Called when the game starts or when spawned
void AShopCoinSlotIndexButton::BeginPlay()
{
	Super::BeginPlay();

	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

	CountCoin = Cast<AShopSlotCoinCountActor>(CoinCountActor->GetChildActor());

	SlotIndexWidgetClass = Cast<UW_ShopCoinSlotIndex>(SlotIndexWidget->GetUserWidgetObject());


	FOnTimelineFloat ButtonPressCallBack;
	ButtonPressCallBack.BindUFunction(this, FName("ButtonMovement"));
	ButtonTimeline->AddInterpFloat(ButtonCurve, ButtonPressCallBack);

	SlotIndexWidgetClass->SetSlotIndex(SlotIndexNum+1);

	ButtonStartVector = ButtonMesh->GetRelativeLocation();
	ButtonArriveVector = ButtonStartVector + ButtonMoveDirection;

	CountCoin->InitCoin(SlotIndexNum);

}
void AShopCoinSlotIndexButton::ButtonMovement(float Value)
{
	FVector TargetVec = FMath::Lerp(ButtonStartVector, ButtonArriveVector, Value);
	ButtonMesh->SetRelativeLocation(TargetVec);
}

void AShopCoinSlotIndexButton::InteractLeftClick_Implementation()
{
	ShopCoinSubsystem->ChangeCoinSlotByIndex(SlotIndexNum);
	ButtonTimeline->PlayFromStart();
}