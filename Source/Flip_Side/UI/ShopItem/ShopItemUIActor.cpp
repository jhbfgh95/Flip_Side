#include "UI/ShopItem/ShopItemUIActor.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
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

    ShopItemSubSystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
	ShopItemSubSystem->OnItemHovered.AddDynamic(this, &AShopItemUIActor::SetItemData);
	ShopItemSubSystem->OnItemBuy.AddDynamic(this, &AShopItemUIActor::BuyItem);
    FOnTimelineFloat ItemBuyCallBack;
	ItemBuyCallBack.BindUFunction(this, FName("ItemBuyMovement"));
	ItemBuyTimeline->AddInterpFloat(ItemBuyCurve, ItemBuyCallBack);	

	StartRotator  = UseableItemMesh->GetRelativeRotation();
	ArriveRotator = StartRotator+ItemShakeRotator;

}

void AShopItemUIActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	ShopItemSubSystem->OnItemHovered.RemoveAll(this);
	ShopItemSubSystem->OnItemBuy.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void AShopItemUIActor::ItemBuyMovement(float Value)
{
	FRotator MoveRotator = FMath::Lerp(StartRotator, ArriveRotator, Value);

	UseableItemMesh->SetRelativeRotation(MoveRotator);
}

void AShopItemUIActor::SetItemMaterial()
{
    UMaterialInstanceDynamic* MID = UseableItemMesh->CreateDynamicMaterialInstance(0);
	
	if(MID && ShopItemData.ItemIcon)
	{	
		MID->SetTextureParameterValue(FName("UseItem_Icon"), ShopItemData.ItemIcon);
		MID->SetVectorParameterValue(FName("Front_Color"), ShopItemData.TypeColor);
	}
	else
	{
		MID->SetTextureParameterValue(FName("UseItem_Icon"), nullptr);
		MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0,0,0));
	}
}

void AShopItemUIActor::SetItemData(FItemData SelectItemData)
{
	ShopItemData = SelectItemData;
	SetItemMaterial();
}

void AShopItemUIActor::BuyItem(int32 Index)
{
	ItemBuyTimeline->PlayFromStart();
}