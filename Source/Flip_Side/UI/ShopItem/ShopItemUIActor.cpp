#include "UI/ShopItem/ShopItemUIActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TimelineComponent.h"

AShopItemUIActor::AShopItemUIActor()
{
    PrimaryActorTick.bCanEverTick = true;

	UseableItemRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = UseableItemRootComp;

	UseableItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UseableItem Mesh"));
	UseableItemMesh->SetupAttachment(RootComponent);

	ItemBuyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ItemBuyTimeline"));
}


void AShopItemUIActor::BeginPlay()
{
    Super::BeginPlay();

    MID = UseableItemMesh->CreateDynamicMaterialInstance(0);

    FOnTimelineFloat ItemBuyCallBack;
	ItemBuyCallBack.BindUFunction(this, FName("ItemBuyMovement"));
	ItemBuyTimeline->AddInterpFloat(ItemBuyCurve, ItemBuyCallBack);	

	StartRotator  = UseableItemMesh->GetRelativeRotation();
	ArriveRotator = StartRotator+ItemShakeRotator;

}

void AShopItemUIActor::ItemBuyMovement(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);

	UseableItemMesh->SetRelativeRotation(MoveRotator);
}

void AShopItemUIActor::SetItemMaterial()
{
	
	if(MID && ShopItemData.ItemIcon)
	{	
		MID->SetTextureParameterValue(FName("UseItem_Icon"), ShopItemData.ItemIcon);
		MID->SetVectorParameterValue(FName("Front_Color"), ShopItemData.TypeColor);
	}
	else
	{
		RemoveMaterial();
	}
}

void AShopItemUIActor::SetItemData(const FItemData& SelectItemData)
{
	ShopItemData = SelectItemData;
	SetItemMaterial();
}

void AShopItemUIActor::PlayBuyAnimation()
{
	if (ItemBuyTimeline)
	{
		ItemBuyTimeline->PlayFromStart();
	}
}


void AShopItemUIActor::RemoveMaterial()
{
	if (MID)
	{
		MID->SetTextureParameterValue(FName("UseItem_Icon"), nullptr);
		MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0,0,0));
	}
}
