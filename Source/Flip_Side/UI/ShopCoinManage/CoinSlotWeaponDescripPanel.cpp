// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/CoinSlotWeaponDescripPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/W_WeaponDescription.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
// Sets default values
ACoinSlotWeaponDescripPanel::ACoinSlotWeaponDescripPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);

	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DescripPanel"));
	PanelMesh->SetupAttachment(RootComponent);

	
	DescripWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DescripWidget"));
	DescripWidget->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void ACoinSlotWeaponDescripPanel::BeginPlay()
{
	Super::BeginPlay();

	CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
	DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

	CoinSubsystem->OnChangeSlotCoinSide.AddDynamic(this, &ACoinSlotWeaponDescripPanel::SetPanel);
	CoinSubsystem->OnCoinSlotChange.AddDynamic(this, &ACoinSlotWeaponDescripPanel::SetPanelChangeSlot);
	CoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &ACoinSlotWeaponDescripPanel::SetPanelUnlockSlot);
	CoinSubsystem->OnCoinCreated.AddDynamic(this, &ACoinSlotWeaponDescripPanel::SetPanelCoinCreate);

	DescripWidgetClass = Cast<UW_WeaponDescription>(DescripWidget->GetUserWidgetObject());

	SetPanel(true);
}

void ACoinSlotWeaponDescripPanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CoinSubsystem->OnChangeSlotCoinSide.RemoveAll(this);
	CoinSubsystem->OnCoinSlotChange.RemoveAll(this);
	CoinSubsystem->OnUnlockCoinSlot.RemoveAll(this);
	CoinSubsystem->OnCoinCreated.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ACoinSlotWeaponDescripPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACoinSlotWeaponDescripPanel::SetPanel(bool IsSlotCoinFront)
{
	FCoinTypeStructure CoinData = CoinSubsystem->GetCurrentSlotCoin();

	if(!CoinSubsystem->GetCurrentCoinUnlock())
	{
		DescripWidgetClass->SetExplainText(TEXT(""), TEXT("슬롯을 해금하세요"),-1,-1);
	}
	else
	{
		FFaceData FaceData;
		if(CoinData.FrontWeaponID == -1 || CoinData.BackWeaponID == -1)
		{
			DescripWidgetClass->SetExplainText(TEXT(""), TEXT("양면에 무기가 장착되지 않았습니다. 해당코인은 전투에 참여하지 않습니다."),-1,-1);
		}
		else
		{
			if(IsSlotCoinFront)
			{
				if(CoinData.FrontWeaponID != -1)
				{
					DataManager->TryGetWeapon(CoinData.FrontWeaponID, FaceData);
					SetPanelDescrip(FaceData);
				}
			}
			else
			{
				if(CoinData.BackWeaponID != -1)
				{
					DataManager->TryGetWeapon(CoinData.BackWeaponID, FaceData);
					SetPanelDescrip(FaceData);
				}
			}
		}
		
		
	}
	
}

void ACoinSlotWeaponDescripPanel::SetPanelUnlockSlot()
{
	SetPanel(true);
}
void ACoinSlotWeaponDescripPanel::SetPanelChangeSlot()
{
	SetPanel(true);
}
void ACoinSlotWeaponDescripPanel::SetPanelCoinCreate(int32 CreatedCoinIndex, EWeaponClass CreateWeaponClass)
{
	SetPanel(true);
}


void ACoinSlotWeaponDescripPanel::SetPanelDescrip(FFaceData FaceData)
{
	if(FaceData.WeaponID != -1)
		DescripWidgetClass->SetExplainText(FaceData.WeaponName, FaceData.KOR_DES, FaceData.BehaviorPoint, FaceData.AttackPoint);
	else
		DescripWidgetClass->SetExplainTextEmpty();
}