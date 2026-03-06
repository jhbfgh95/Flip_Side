// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUseableItemActor.h"
#include "Subsystem/ShopItemWSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TimelineComponent.h"

AShopUseableItemActor::AShopUseableItemActor()
{
    	PrimaryActorTick.bCanEverTick = true;

	UseableItemRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = UseableItemRootComp;

	UseableItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UseableItem Mesh"));
	UseableItemMesh->SetupAttachment(RootComponent);

	ItemExplainMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemExplain Mesh"));
	ItemExplainMesh->SetupAttachment(RootComponent);

	ItemMeshTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ItemMeshTimeline"));
}


void AShopUseableItemActor::BeginPlay()
{
    Super::BeginPlay();

    ShopItemSubSystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

   
    if(ShopItemSubSystem)
    {
        //해당 소모품 정보를 받아옴
        //ShopItemData = ShopItemSubSystem->GetItemDataByShopIndex(ShopItemIndex);
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

void AShopUseableItemActor::HoverdItem()
{
	ItemMeshTimeline->PlayFromStart();
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
    ShopItemSubSystem->BuyItemByIndex(ShopItemIndex);
}    

void AShopUseableItemActor::HoveredUseAbleItem()
{
    //마우스올라갔을 떄 설명 띄우기
    ShopItemSubSystem->HoverShopItem(ShopItemIndex);
    //추후 움직임 에니메이션 추가
}

void AShopUseableItemActor::UnHoveredUseAbleItem()
{
    //마우스 내려 갔을 때 설명내리기
    ShopItemSubSystem->UnHoverShopItem();

    //추후 움직임 에니메이션 추가
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