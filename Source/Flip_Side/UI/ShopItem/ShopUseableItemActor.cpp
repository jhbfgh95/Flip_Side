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
    SetItemMaterial();

    FOnTimelineFloat ItemMoveCallBack;
	ItemMoveCallBack.BindUFunction(this, FName("ItemHoverMovement"));
	ItemMeshTimeline->AddInterpFloat(ItemMoveCurve, ItemMoveCallBack);	
	
    FOnTimelineFloat ItemDescriptionMoveCallBack;
	ItemDescriptionMoveCallBack.BindUFunction(this, FName("ItemDescriptionMovement"));
	ItemMeshTimeline->AddInterpFloat(DescriptionMoveCurve, ItemDescriptionMoveCallBack);	
	

	StartLocation = UseableItemMesh->GetRelativeLocation();
	ArriveLocation = StartLocation + ItemMoveDirection;

	ItemDescriptionStartLocation = ItemExplainMesh->GetRelativeLocation();
	ItemDescriptionArriveLocation = ItemDescriptionStartLocation + ItemDescriptionMoveDirection;

}


void AShopUseableItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
void AShopUseableItemActor::HoverdItem()
{
	ItemMeshTimeline->Play();
}
void AShopUseableItemActor::UnHoverdItem()
{
	ItemMeshTimeline->Reverse();
}

void AShopUseableItemActor::ItemHoverMovement(float Value)
{
	FVector MoveVector = FMath::Lerp(StartLocation, ArriveLocation, Value);

	UseableItemMesh->SetRelativeLocation(MoveVector);
}

void AShopUseableItemActor::ItemDescriptionMovement(float Value)
{
	FVector MoveVector = FMath::Lerp(ItemDescriptionStartLocation, ItemDescriptionArriveLocation, Value);

	ItemExplainMesh->SetRelativeLocation(MoveVector);
}

void AShopUseableItemActor::LClickedUseAbleItem()
{
    //상점 아이템 구입
    ShopItemSubSystem->BuyItem(ShopItemData);
}    

void AShopUseableItemActor::HoveredUseAbleItem()
{
    //마우스올라갔을 떄 설명 띄우기
    ShopItemSubSystem->HoverShopItem(ShopItemData);
}

void AShopUseableItemActor::UnHoveredUseAbleItem()
{
    //마우스 내려 갔을 때 설명내리기
    ShopItemSubSystem->UnHoverShopItem();
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
	ShopItemSubSystem->BuyItem(ShopItemData);
}