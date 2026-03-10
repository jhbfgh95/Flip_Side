// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/SelectClassLever.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"

ASelectClassLever::ASelectClassLever()
{
	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
	InteractBox->SetupAttachment(RootComponent);
}
// Called when the game starts or when spawned
void ASelectClassLever::BeginPlay()
{
	Super::BeginPlay();

	CoinCreateSubsystem = GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

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

void ASelectClassLever::ExecuteLeverLogic()
{
	CoinCreateSubsystem->SetCoinClass(LeverClass);
}

void ASelectClassLever::InteractLeftClick_Implementation()
{
	OnLeverInteracted();
}