// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUISelectActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AShopUISelectActor::AShopUISelectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SelectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectMesh"));
	SelectMesh->SetupAttachment(SceneRoot);

	ClickCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ClickCollision"));
	ClickCollision->SetupAttachment(SceneRoot);
	ClickCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClickCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClickCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ClickCollision->ComponentTags.Add(TEXT("LClickAble"));
	ClickCollision->ComponentTags.Add(TEXT("HoverAble"));
	SelectWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("SelectWidget"));
	SelectWidget->SetupAttachment(SceneRoot);

}

// Called when the game starts or when spawned
void AShopUISelectActor::BeginPlay()
{
	Super::BeginPlay();
	SelectWidget->SetVisibility(false);
	
}

void AShopUISelectActor::SetClickCollisionEnabled(bool bEnabled)
{
	if (!IsValid(ClickCollision))
	{
		return;
	}

	ClickCollision->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	if (!bEnabled && IsValid(SelectWidget))
	{
		SelectWidget->SetVisibility(false);
	}
}

void AShopUISelectActor::InteractLeftClick_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Shop UI Select Actor Left Click"));
	OnClickShopPageChangeActor.Broadcast(ConnectedPage);
}

void AShopUISelectActor::InteractRightClick_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Shop UI Select Actor Right Click"));
}

void AShopUISelectActor::InteractHover_Implementation()
{
	OnHoverShopPageChangeActor.Broadcast(ConnectedPage);
	SelectWidget->SetVisibility(true);
}

void AShopUISelectActor::InteractUnHover_Implementation()
{
	OnUnhoverShopPageChangeActor.Broadcast();
	SelectWidget->SetVisibility(false);
}
