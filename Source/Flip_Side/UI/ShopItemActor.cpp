// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
// Sets default values
AShopItemActor::AShopItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ItemMesh->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AShopItemActor::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnBeginCursorOver.AddDynamic(this, &AShopItemActor::OnHoverBegin);
    SphereCollision->OnEndCursorOver.AddDynamic(this, &AShopItemActor::OnHoverEnd);
}

// Called every frame
void AShopItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShopItemActor::OnHoverBegin(UPrimitiveComponent* TouchedComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("올라감"));
}

void AShopItemActor::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("올라가지 않음"));
}
