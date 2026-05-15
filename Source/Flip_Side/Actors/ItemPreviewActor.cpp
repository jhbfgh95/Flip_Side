// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemPreviewActor.h"
#include "UseableItemActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AItemPreviewActor::AItemPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = PreviewRootComp;

	PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Preview Mesh"));
	PreviewMesh->SetupAttachment(RootComponent);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMesh->SetGenerateOverlapEvents(false);

	SetActorEnableCollision(false);
}

void AItemPreviewActor::InitFromItem(AUseableItemActor* SourceItem)
{
	if(!SourceItem || !PreviewMesh) return;

	UStaticMeshComponent* SourceMesh = SourceItem->GetUseableItemMesh();
	if(!SourceMesh) return;

	PreviewMesh->SetStaticMesh(SourceMesh->GetStaticMesh());

	FTransform PreviewTransform = SourceMesh->GetRelativeTransform();
	PreviewTransform.SetScale3D(PreviewTransform.GetScale3D() * PreviewScaleMultiplier);
	PreviewMesh->SetRelativeTransform(PreviewTransform);

	const int32 MaterialCount = SourceMesh->GetNumMaterials();
	for(int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		PreviewMesh->SetMaterial(MaterialIndex, SourceMesh->GetMaterial(MaterialIndex));
	}
}

void AItemPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemPreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

