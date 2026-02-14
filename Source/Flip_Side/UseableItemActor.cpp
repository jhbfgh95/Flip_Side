#include "UseableItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
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

	if(ItemTexture)
	{
		UMaterialInstanceDynamic* MID = UseableItemMesh->CreateDynamicMaterialInstance(0);
		
		if(MID)
		{	
			MID->SetTextureParameterValue(FName("UseItem_Icon"), ItemTexture);
			MID->SetVectorParameterValue(FName("Front_Color"), ItemColor);
		}
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

void AUseableItemActor::SetItemValues(int32 TheItemID, UTexture2D* ItemTex, FLinearColor Color)
{
	ItemID = TheItemID;
	ItemTexture = ItemTex;
	ItemColor = Color;
}