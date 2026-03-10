// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/UnlockWeaponLever.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Components/BoxComponent.h"

AUnlockWeaponLever::AUnlockWeaponLever()
{
    InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
    InteractBox->SetupAttachment(RootComponent);
}
void AUnlockWeaponLever::BeginPlay()
{
    Super::BeginPlay();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();

}

void AUnlockWeaponLever::ExecuteLeverLogic()
{
    UnlockWeaponSubsystem->UnlockCurrentWeapon();
}
	
 
void AUnlockWeaponLever::InteractLeftClick_Implementation()
{
    OnLeverInteracted();
}