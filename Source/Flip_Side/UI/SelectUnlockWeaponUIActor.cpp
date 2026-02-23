// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectUnlockWeaponUIActor.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/ShopWeaponDataWSubsystem.h"
// Sets default values
ASelectUnlockWeaponUIActor::ASelectUnlockWeaponUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMeshComponent"));
}

// Called when the game starts or when spawned
void ASelectUnlockWeaponUIActor::BeginPlay()
{
	Super::BeginPlay();
	ShopUnlockWeaponSubSystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
	WeaponDataSubsystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();

	
}

// Called every frame
void ASelectUnlockWeaponUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASelectUnlockWeaponUIActor::SetUnlockCoin(EWeaponClass WeaponClass, int32 index, bool IsItemUnlock)
{
}