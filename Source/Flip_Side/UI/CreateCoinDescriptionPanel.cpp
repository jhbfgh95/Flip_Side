// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreateCoinDescriptionPanel.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/W_WeaponDescription.h"

// Sets default values
ACreateCoinDescriptionPanel::ACreateCoinDescriptionPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
	RootSecen =CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneCompoenent"));

	SetRootComponent(RootSecen);

	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));

	PanelMesh->SetupAttachment(RootComponent);

	ExplainWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ExplainWidget"));
	ExplainWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACreateCoinDescriptionPanel::BeginPlay()
{
	Super::BeginPlay();

	CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	if(CoinCreateWSubSystem)
	{
		CoinCreateWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &ACreateCoinDescriptionPanel::GetExplainByID);
	}
	ExplainWidgetClass = Cast<UW_WeaponDescription>(ExplainWidget->GetUserWidgetObject());

}

void ACreateCoinDescriptionPanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CoinCreateWSubSystem->OnSelectedCoinUpdate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ACreateCoinDescriptionPanel::GetExplainByID(int32 ID, bool IsFront)
{
	SetDescriptionText(ID);
}

void ACreateCoinDescriptionPanel::SetDescriptionText(int32 ID)
{
	
	//추후 무기 설명 반환하는 코드작성
	FFaceData WeaponData;
	DataManagerSubsystem->TryGetWeapon(ID, WeaponData);

	ExplainWidgetClass->SetExplainText(WeaponData.WeaponName, WeaponData.KOR_DES, WeaponData.BehaviorPoint, WeaponData.AttackPoint);
}