// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/ShopCoinManagePanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TimelineComponent.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Components/WidgetComponent.h"
#include "DataTypes/CoinDataTypes.h"

#include "WeaponDataTypes.h"
#include "Subsystem/DataManagerSubsystem.h"


#include "UI/ShopCoinManage/W_CoinManagePanelWidget.h"
#include "UI/ShopCoinManage/CoinSlotLockPanel.h"
#include "UI/ShopCoinManage/ShopCoinManageCoin.h"
#include "UI/ShopCoinManage/ShopCoinSlotCountButton.h"
#include "UI/W_WeaponDescription.h"
#include "UI/ShopCoinManage/CoinSlotWeaponDescripPanel.h"
// Sets default values
AShopCoinManagePanel::AShopCoinManagePanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
    //스태틱 메쉬들
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Panel"));
    PanelMesh->SetupAttachment(RootScene);

	GearActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("GearActor"));
	GearActor->SetupAttachment(RootScene);


	
	IncreaseCountButton = CreateDefaultSubobject<UChildActorComponent>(TEXT("IncreaseCountButton"));
	IncreaseCountButton->SetupAttachment(RootScene);

	DecreaseCountButton= CreateDefaultSubobject<UChildActorComponent>(TEXT("DecreaseCountButton"));
	DecreaseCountButton->SetupAttachment(RootScene);

	CoinActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("CoinActor"));
	CoinActor->SetupAttachment(RootScene);

	LockPanel= CreateDefaultSubobject<UChildActorComponent>(TEXT("LockPanel"));
	LockPanel->SetupAttachment(RootScene);


	//위젯들
	PanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PanelWidget"));
	PanelWidget->SetupAttachment(RootScene);



	

}

// Called when the game starts or when spawned
void AShopCoinManagePanel::BeginPlay()
{
	Super::BeginPlay();

	ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this, &AShopCoinManagePanel::SetPanelWidget);
	
	ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &AShopCoinManagePanel::InitPanel);

	//Set Actor Class
	IncreaseCountButtonClass = Cast<AShopCoinSlotCountButton>(IncreaseCountButton->GetChildActor());;
	DecreaseCountButtonClass = Cast<AShopCoinSlotCountButton>(DecreaseCountButton->GetChildActor());
	LockPanelClass= Cast<ACoinSlotLockPanel>(LockPanel->GetChildActor());
	CoinActorClass= Cast<AShopCoinManageCoin>(CoinActor->GetChildActor());


	PanelWidgetClass = Cast<UW_CoinManagePanelWidget>(PanelWidget->GetUserWidgetObject());

	LockPanelStartVector = LockPanel->GetRelativeLocation();
}

void AShopCoinManagePanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopCoinSubsystem->OnCoinSlotChange.RemoveAll(this);
	ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void AShopCoinManagePanel::InitPanel()
{
	
	if(ShopCoinSubsystem->GetCurrentCoinUnlock())
		
		LockPanelClass->InitLockPanel(GetActorLocation()-LockPanelStartVector + LockPanelClass->GetUnlockPanelDirection());
	else
		LockPanelClass->InitLockPanel(GetActorLocation()-LockPanelStartVector);
		
	CoinActorClass->InitCoin();
}

void AShopCoinManagePanel::SetPanelWidget(int32 CoinSlotIndex, int32 CoinCount)
{
	PanelWidgetClass->SetCountText(CoinCount);
}


void AShopCoinManagePanel::InitPanelCoin()
{
	CoinActorClass->InitCoin();
}