#include "UseableItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "FlipSide_Enum.h"
#include "Materials/MaterialInstanceDynamic.h"

AUseableItemActor::AUseableItemActor()
{
	PrimaryActorTick.bCanEverTick = true;

	UseableItemRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = UseableItemRootComp;

	UseableItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UseableItem Mesh"));
	UseableItemMesh->SetupAttachment(RootComponent);
}

void AUseableItemActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyItemVisual();
}


void AUseableItemActor::ApplyItemVisual()


{
	if(!IsValid(UseableItemMesh) || !IsValid(ItemTexture))
	{
		return;
	}

	UMaterialInstanceDynamic* MID = UseableItemMesh->CreateDynamicMaterialInstance(0);
	if(IsValid(MID))
	{
		MID->SetTextureParameterValue(FName("UseItem_Icon"), ItemTexture);
		MID->SetVectorParameterValue(FName("Front_Color"), ItemColor);
	}
}

void AUseableItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUseableItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUseableItemActor::SetItemValues(int32 TheItemID, EItemType theItemType,UTexture2D* ItemTex, FLinearColor Color, int32 price)
{
	ItemID = TheItemID;
	ItemType = theItemType;
	ItemTexture = ItemTex;
	ItemColor = Color;
	ItemPrice = price;
	ApplyItemVisual();
}

void AUseableItemActor::InitializeAsCursorPreview(const FItemData& ItemData, float PreviewScaleMultiplier)
{
	SetItemValues(ItemData.ItemID, ItemData.ItemType, ItemData.ItemIcon, ItemData.TypeColor, ItemData.Price);

	SetActorEnableCollision(false);
	if(IsValid(UseableItemMesh))
	{
		UseableItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UseableItemMesh->SetGenerateOverlapEvents(false);
		UseableItemMesh->SetRelativeScale3D(UseableItemMesh->GetRelativeScale3D() * PreviewScaleMultiplier);
	}
}

int32 AUseableItemActor::GetItemID() const
{
	return ItemID;
}

void AUseableItemActor::OnHover_Implementation()
{
	OnHoverItem.Broadcast(this);
}

void AUseableItemActor::OnUnhover_Implementation()
{
	OnUnhoverItem.Broadcast();
}

void AUseableItemActor::OnClicked_Implementation()
{
	if(ItemType == EItemType::CoinBuff)
	{
		OnCoinClickItem.Broadcast(this);
	}
	else if(ItemType == EItemType::Install)
	{
		OnGridClickItem.Broadcast(this);
	}
	else if(ItemType == EItemType::OtherBuff)
	{
		//Others는.. 매니저가 없어서 만들던가 해야할 듯 하네
		UE_LOG(LogTemp, Warning, TEXT("아직 미구현입니다."));
		return;
	}
}

void AUseableItemActor::OnRightClicked_Implementation()
{
	OnItemRightClick.Broadcast(this);
}
