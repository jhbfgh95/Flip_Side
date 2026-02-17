// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectClassLever.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Materials/MaterialInstanceDynamic.h"

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