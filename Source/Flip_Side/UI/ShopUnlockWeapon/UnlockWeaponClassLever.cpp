// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/UnlockWeaponClassLever.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"

AUnlockWeaponClassLever::AUnlockWeaponClassLever()
{
	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
	InteractBox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AUnlockWeaponClassLever::BeginPlay()
{
	Super::BeginPlay();

	UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();

	UMaterialInstanceDynamic* MID = LeverMesh->CreateDynamicMaterialInstance(1);
	if(MID)
	{
		switch(LeverClass)
		{
			case EWeaponClass::Tank:
				MID->SetVectorParameterValue(FName("TypeColor"), TankClassColor);
				break;
			case EWeaponClass::Deal:
				MID->SetVectorParameterValue(FName("TypeColor"), DealClassColor);
				break;
			case EWeaponClass::Heal:
				MID->SetVectorParameterValue(FName("TypeColor"), UtilClassColor);
				break;
			default:
				MID->SetVectorParameterValue(FName("TypeColor"), DefaultColor);
				break;
		}
		
	}

}

void AUnlockWeaponClassLever::ExecuteLeverLogic()
{
	UnlockWeaponSubsystem->ChangeUnlockWeaponClass(LeverClass);
}

void AUnlockWeaponClassLever::InteractLeftClick_Implementation()
{
	OnLeverInteracted();
}