// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinSlotSelectButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "SubSystem/ShopCoinWSubsystem.h"

// Sets default values
AShopCoinSlotSelectButton::AShopCoinSlotSelectButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
    
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(RootScene);

	ButtonTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("ButtonTimeLine"));

	CoinCountWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CoinCountWidget"));
	CoinCountWidget->SetupAttachment(ButtonMesh);
}

// Called when the game starts or when spawned
void AShopCoinSlotSelectButton::BeginPlay()
{
	Super::BeginPlay();
	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this, &AShopCoinSlotSelectButton::SetCountWidget);
}

// Called every frame
void AShopCoinSlotSelectButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void AShopCoinSlotSelectButton::SelectCoin()
{
	//ShopCoinSubsystem 쪽에서 코인 선택하는 델리게이트 실행하는 방법으로
	ShopCoinSubsystem->ChangeCoinSlotByIndex(SlotIndexNum);
}


void AShopCoinSlotSelectButton::InitButton()
{
	//버튼의 위젯의 숫자 설정
	//색깔 설정
}


void AShopCoinSlotSelectButton::SetCountWidget(int32 SlotIndex, int32 Count)
{
	//위젯에서 설정
}