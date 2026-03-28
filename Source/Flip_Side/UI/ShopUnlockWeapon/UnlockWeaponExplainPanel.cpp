// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/UnlockWeaponExplainPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/W_WeaponDescription.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
// Sets default values
AUnlockWeaponExplainPanel::AUnlockWeaponExplainPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;
	ExplainPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplainMesh"));
	ExplainPanel->SetupAttachment(RootComponent);
	ExplainWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ExplainWidget"));
	ExplainWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AUnlockWeaponExplainPanel::BeginPlay()
{
	Super::BeginPlay();
	ExplainWidgetClass = Cast<UW_WeaponDescription>(ExplainWidget->GetUserWidgetObject());
	ShopUnlockSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
	DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	ShopUnlockSubsystem->OnSelectUnlockWeapon.AddDynamic(this, &AUnlockWeaponExplainPanel::SetUnlockWeaponText);
	
}

void AUnlockWeaponExplainPanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShopUnlockSubsystem->OnSelectUnlockWeapon.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}
	

void AUnlockWeaponExplainPanel::SetUnlockWeaponText(EWeaponClass WeaponClass ,int32 ItemID, bool IsItemUnlock)
{

	//추후 무기 설명 반환하는 코드작성
	FFaceData WeaponData;
	DataManagerSubsystem->TryGetWeapon(ItemID, WeaponData);

	ExplainWidgetClass->SetExplainText(WeaponData.WeaponName, WeaponData.KOR_DES, WeaponData.BehaviorPoint, WeaponData.AttackPoint);
}