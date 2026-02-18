// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUseableItemActor.h"
#include "Subsystem/ShopItemWSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AShopUseableItemActor::AShopUseableItemActor()
{
    	PrimaryActorTick.bCanEverTick = true;

	UseableItemRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = UseableItemRootComp;

	UseableItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UseableItem Mesh"));
	UseableItemMesh->SetupAttachment(RootComponent);
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
}


void AShopUseableItemActor::LClickedUseAbleItem()
{
    //상점 아이템 구입
    ShopItemSubSystem->BuyItemByIndex(ShopItemIndex);
}    
    
void AShopUseableItemActor::RClickedUseAbleItem()
{

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