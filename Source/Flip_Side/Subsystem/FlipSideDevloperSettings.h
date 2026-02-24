// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlipSideDevloperSettings.generated.h"

class ACoinActor;
class ASlotActor;
class AUseableItemActor;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="My Manager Settings"))
class FLIP_SIDE_API UFlipSideDevloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "CoinSpawn")
	TSoftClassPtr<ACoinActor> CoinActor;

	UPROPERTY(Config, EditAnywhere, Category = "CoinSpawn")
	TSoftClassPtr<ASlotActor> SlotActor;

	UPROPERTY(Config, EditAnywhere, Category = "UseableItemSpawn")
	TSoftClassPtr<AUseableItemActor> UseableItemActor;

	UPROPERTY(Config, EditAnywhere, Category = "UseableItemSpawn")
	TSoftClassPtr<ASlotActor> UseableItemSlotActor;

	UPROPERTY(EditAnywhere, Config, Category = "GridSpawn")
	TSoftClassPtr<class AGridActor> GridActor;

	UPROPERTY(EditAnywhere, config, Category = "Stage UI",
		meta = (AllowedClasses = "/Script/UMG.UserWidget"))
	TSoftClassPtr<UUserWidget> StageHUDWidgetClass;
};
