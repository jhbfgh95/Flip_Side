// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/ShopPlayerItemActor.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SphereComponent.h"

#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"

#include "Subsystem/DataManagerSubsystem.h"
// Sets default values
AShopPlayerItemActor::AShopPlayerItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UseableItemRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = UseableItemRootComp;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UseableItem Mesh"));
	ItemMesh->SetupAttachment(RootComponent);

	ItemWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemCountWidget"));
	ItemWidget->SetupAttachment(ItemMesh);

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(RootComponent);

	ItemDescriptionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemDescriptionMesh"));
	ItemDescriptionMesh->SetupAttachment(ItemMesh);

	ItemMeshTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ItemMeshTimeline"));

}

// Called when the game starts or when spawned
void AShopPlayerItemActor::BeginPlay()
{
	Super::BeginPlay();
	
	ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
	ItemDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

	
	FOnTimelineFloat ItemMoveCallBack;
	ItemMoveCallBack.BindUFunction(this, FName("ItemHoverMovement"));
	ItemMeshTimeline->AddInterpFloat(ItemMoveCurve, ItemMoveCallBack);	

	FOnTimelineFloat DescriptionMoveCallBack;
	DescriptionMoveCallBack.BindUFunction(this, FName("DescriptionMovement"));
	ItemMeshTimeline->AddInterpFloat(DescriptionCurve, DescriptionMoveCallBack);	


	StartLocation = ItemMesh->GetRelativeLocation();
	ArriveLocation = StartLocation + ItemMoveDirection;

	DescriptionStartLocation = ItemDescriptionMesh->GetRelativeLocation();
	DescriptionArriveLocation = DescriptionStartLocation + DescriptionMoveDirection;

}

void AShopPlayerItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AShopPlayerItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShopPlayerItemActor::InitItem(int32 Index)
{
	PlayerSelectItemData = ShopItemSubsystem->GetPlayerItem(Index);
	if(PlayerSelectItemData.ItemID != -1)
	{
		if(ItemDataSubsystem->TryGetItem(PlayerSelectItemData.ItemID, PlayerItemData))
		{
			SetItemMaterial();
		}
		
	}
	else
	{
		//HideItem();
	}
}

void AShopPlayerItemActor::SetItemMaterial()
{
    UMaterialInstanceDynamic* MID = ItemMesh->CreateDynamicMaterialInstance(0);
	
	if(MID && PlayerItemData.ItemIcon)
	{	
		MID->SetTextureParameterValue(FName("UseItem_Icon"), PlayerItemData.ItemIcon);
		MID->SetVectorParameterValue(FName("Front_Color"), PlayerItemData.TypeColor);
	}
}

void AShopPlayerItemActor::HoverdItem()
{
	ItemMeshTimeline->Play();
}
void AShopPlayerItemActor::UnHoverdItem()
{
	ItemMeshTimeline->Reverse();
}
	

void AShopPlayerItemActor::ItemHoverMovement(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, ArriveLocation, Value);

	ItemMesh->SetRelativeLocation(MoveVector);
}

void AShopPlayerItemActor::DescriptionMovement(float Value)
{
	FVector MoveVector = FMath::Lerp(DescriptionStartLocation, DescriptionArriveLocation, Value);

	ItemDescriptionMesh->SetRelativeLocation(MoveVector);
}

void AShopPlayerItemActor::InteractHover_Implementation()
{
	HoverdItem();
}

void AShopPlayerItemActor::InteractUnHover_Implementation()
{
	UnHoverdItem();
}
void AShopPlayerItemActor::HideItem()
{
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);
}