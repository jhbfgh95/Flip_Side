// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManageActor.h"
#include "Components/TimelineComponent.h"
#include "UI/ShopCoinManagePanel.h"
#include "Subsystem/ShopCoinWSubsystem.h"
// Sets default values
AShopCoinManageActor::AShopCoinManageActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);

	CoinMPanel1 = CreateDefaultSubobject<UChildActorComponent>(TEXT("TopPanelComponent"));
	CoinMPanel1->SetupAttachment(RootComponent);

	CoinMPanel2 = CreateDefaultSubobject<UChildActorComponent>(TEXT("BottomPanelComponent"));
	CoinMPanel2->SetupAttachment(RootComponent);

	MoveToBottomTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MoveToBottomTimeline"));
	MoveToTopTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MoveToTopTimeline"));
}

// Called when the game starts or when spawned
void AShopCoinManageActor::BeginPlay()
{
	Super::BeginPlay();
	
	ShopCoinSubsystem= GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &AShopCoinManageActor::ChangePanel);
	ShopCoinSubsystem->OnCoinCreated.AddDynamic(this, &AShopCoinManageActor::InitSlotPanelCoin);
	FOnTimelineFloat MoveToBottomCallBack;
	MoveToBottomCallBack.BindUFunction(this,FName("MovePanelToBottom"));
	MoveToBottomTimeline->AddInterpFloat(PanelMoveCurve, MoveToBottomCallBack);	

	FOnTimelineEvent FinishMoveToBottomCallBack;
	FinishMoveToBottomCallBack.BindUFunction(this, FName("FinishMovePanelToBottom"));
	MoveToBottomTimeline->SetTimelineFinishedFunc(FinishMoveToBottomCallBack);
	
	FOnTimelineFloat MoveToTopCallBack;
	MoveToTopCallBack.BindUFunction(this,FName("MovePanelToTop"));
	MoveToTopTimeline->AddInterpFloat(PanelMoveCurve, MoveToTopCallBack);	

	FOnTimelineEvent FinishMoveToTopCallBack;
	FinishMoveToTopCallBack.BindUFunction(this, FName("FinishMovePanelToTop"));
	MoveToTopTimeline->SetTimelineFinishedFunc(FinishMoveToTopCallBack);

	ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &AShopCoinManageActor::UnlockCurrentPanel);


	CoinMPanel1->SetRelativeLocation(ReadyTopVector);
	CurrentTopPanel = CoinMPanel1;
	CurrentBottomPanel = CoinMPanel1;


	CoinMPanel2->SetRelativeLocation(ShowVector);
	CurrentShowPanel = CoinMPanel2;

	Panel1Class = Cast<AShopCoinManagePanel>(CoinMPanel1->GetChildActor());
	Panel2Class = Cast<AShopCoinManagePanel>(CoinMPanel2->GetChildActor());
}


void AShopCoinManageActor::ChangePanelToBottom()
{
	if(CurrentShowPanel == CoinMPanel1)
	{
		Panel2Class->InitPanel();
		Panel1Class->InitPanelToStart();
	}
	else
	{
		Panel1Class->InitPanel();
		Panel2Class->InitPanelToStart();
	}
	MoveToBottomTimeline->PlayFromStart();
}

void AShopCoinManageActor::ChangePanelToTop()
{
	if(CurrentShowPanel == CoinMPanel1)
	{
		Panel2Class->InitPanel();
		Panel1Class->InitPanelToStart();
	}
	else
	{
		Panel1Class->InitPanel();
		Panel2Class->InitPanelToStart();
	}
	
	MoveToTopTimeline->PlayFromStart();
}


void AShopCoinManageActor::MovePanelToBottom(float Value)
{
	FVector TopToShowVec = FMath::Lerp(ReadyTopVector, ShowVector, Value);
	FVector ShowToBottomVec = FMath::Lerp(ShowVector, ReadyBottomVector, Value);

	CurrentTopPanel->SetRelativeLocation(TopToShowVec);
	CurrentShowPanel->SetRelativeLocation(ShowToBottomVec);
}

void AShopCoinManageActor::FinishMovePanelToBottom()
{
	CurrentBottomPanel = CurrentShowPanel;
	CurrentShowPanel = CurrentTopPanel; 
	CurrentTopPanel = CurrentBottomPanel;

	if(CurrentShowPanel == CoinMPanel1)
	{
		Panel1Class->InitPanelAfterArrive();
	}
	else
	{
		Panel2Class->InitPanelAfterArrive();
	}
}


void AShopCoinManageActor::MovePanelToTop(float Value)
{
	FVector BottomToShowVec = FMath::Lerp(ReadyBottomVector, ShowVector, Value);
	FVector ShowToTopVec = FMath::Lerp(ShowVector, ReadyTopVector, Value);

	CurrentShowPanel->SetRelativeLocation(ShowToTopVec);
	CurrentBottomPanel->SetRelativeLocation(BottomToShowVec);
}

void AShopCoinManageActor::FinishMovePanelToTop()
{
	CurrentTopPanel = CurrentShowPanel;
	CurrentShowPanel = CurrentBottomPanel;
	CurrentBottomPanel = CurrentTopPanel;

	if(CurrentShowPanel == CoinMPanel1)
	{
		Panel1Class->InitPanelAfterArrive();
	}
	else
	{
		Panel2Class->InitPanelAfterArrive();
	}
}

void AShopCoinManageActor::ChangePanel(bool IsChangeToBottom)
{
	if(IsChangeToBottom)
		ChangePanelToBottom();
	else
		ChangePanelToTop();
}


void AShopCoinManageActor::UnlockCurrentPanel()
{
	if(CurrentShowPanel == CoinMPanel1)
	{
		Panel1Class->UnlockPanel();
	}
	else
	{
		Panel2Class->UnlockPanel();
	}
}

void AShopCoinManageActor::ClickChangeSlotTop()
{
	ShopCoinSubsystem->ChangeCoinSlotLeft();
}

void AShopCoinManageActor::ClickChangeSlotBottom()
{
	ShopCoinSubsystem->ChangeCoinSlotRight();
}

void AShopCoinManageActor::InitSlotPanelCoin(int32 CreateCoinIndex, EWeaponClass WeaponClass)
{
	if(CurrentShowPanel == CoinMPanel1)
	{
		Panel1Class->InitPanelCoin();
	}
	else
	{
		Panel2Class->InitPanelCoin();
	}
}