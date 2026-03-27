// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/ShopUseableItemActor.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TimelineComponent.h"
#include "Components/SphereComponent.h"

AShopUseableItemActor::AShopUseableItemActor()
{
    	PrimaryActorTick.bCanEverTick = true;

	UseableItemRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = UseableItemRootComp;

	UseableItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UseableItem Mesh"));
	UseableItemMesh->SetupAttachment(RootComponent);

	ItemExplainMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemExplain Mesh"));
	ItemExplainMesh->SetupAttachment(RootComponent);

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(RootComponent);

	ItemMeshTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ItemMeshTimeline"));
	ItemBuyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ItemBuyTimeline"));
}


void AShopUseableItemActor::BeginPlay()
{
    Super::BeginPlay();

    ShopItemSubSystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

   
    if(ShopItemSubSystem)
    {
        //해당 소모품 정보를 받아옴
        ShopItemData = ShopItemSubSystem->GetItemDataByShopIndex(ShopItemIndex);
    }
	if(ShopItemData.ItemID == -1)
	{
		
		HideItem();
	}
	else
	{
		ShowItem();
    	SetItemMaterial();
	}

    FOnTimelineFloat ItemMoveCallBack;
	ItemMoveCallBack.BindUFunction(this, FName("ItemHoverMovement"));
	ItemMeshTimeline->AddInterpFloat(ItemMoveCurve, ItemMoveCallBack);	
	
    FOnTimelineFloat ItemBuyCallBack;
	ItemBuyCallBack.BindUFunction(this, FName("ItemBuyMovement"));
	ItemBuyTimeline->AddInterpFloat(ItemBuyCurve, ItemBuyCallBack);	
	

	StartLocation = UseableItemMesh->GetRelativeLocation();
	ArriveLocation = StartLocation + ItemMoveDirection;

	StartRotator  = UseableItemMesh->GetRelativeRotation();
	ArriveRotator = StartRotator+ItemShakeRotator;

}


void AShopUseableItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
void AShopUseableItemActor::HoverdItem()
{
	ShopItemSubSystem->HoverItem(ShopItemData);
	ItemMeshTimeline->Play();
}
void AShopUseableItemActor::UnHoverdItem()
{
	ShopItemSubSystem->UnHoverItem();
	ItemMeshTimeline->Reverse();
}

void AShopUseableItemActor::ItemHoverMovement(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, ArriveLocation, Value);
	
	UseableItemMesh->SetRelativeLocation(MoveVector);
}


void AShopUseableItemActor::ItemBuyMovement(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);

	UseableItemMesh->SetRelativeRotation(MoveRotator);
}

void AShopUseableItemActor::LClickedUseAbleItem()
{
    //상점 아이템 구입
    ShopItemSubSystem->BuyItem(ShopItemData);
	ItemBuyTimeline->PlayFromStart();
}

void AShopUseableItemActor::SetItemMaterial()
{
    UMaterialInstanceDynamic* MID = UseableItemMesh->CreateDynamicMaterialInstance(0);
	
	if(MID && ShopItemData.ItemIcon)
	{	
		MID->SetTextureParameterValue(FName("UseItem_Icon"), ShopItemData.ItemIcon);
		MID->SetVectorParameterValue(FName("Front_Color"), ShopItemData.TypeColor);
	}
}

void AShopUseableItemActor::HideItem()
{
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);
}
	
void AShopUseableItemActor::ShowItem()
{
	this->SetActorHiddenInGame(false);
	this->SetActorEnableCollision(true);
}



void AShopUseableItemActor::InteractHover_Implementation()
{
	HoverdItem();
}

void AShopUseableItemActor::InteractUnHover_Implementation()
{
	UnHoverdItem();
}

void AShopUseableItemActor::InteractLeftClick_Implementation()
{
	ItemBuyTimeline->PlayFromStart();
	ShopItemSubSystem->BuyItem(ShopItemData);
}
