// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BossDataTypes.h"
#include "FlipSideDevloperSettings.generated.h"

class ACoinActor;
class ASlotActor;
class ACoinSlotActor;
class UW_ReadyAndSlotCoinInfo;
class UW_BattleCoinInfo;
class AUseableItemActor;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="My Manager Settings"))
class FLIP_SIDE_API UFlipSideDevloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Coin | Spawn")
	TSoftClassPtr<ACoinActor> CoinActor;

	UPROPERTY(Config, EditAnywhere, Category = "Coin | Spawn")
	TSoftClassPtr<ACoinSlotActor> CoinSlotActor;

	UPROPERTY(EditAnywhere, config, Category = "Coin | UI")
	TSoftClassPtr<UW_ReadyAndSlotCoinInfo> ReadyAndSlotCoinInfoWidget;

	UPROPERTY(EditAnywhere, config, Category = "Coin | UI")
	TSoftClassPtr<UW_BattleCoinInfo> BattleCoinInfoWidget;

	UPROPERTY(Config, EditAnywhere, Category = "UseableItem | Spawn")
	TSoftClassPtr<AUseableItemActor> UseableItemActor;

	UPROPERTY(Config, EditAnywhere, Category = "UseableItem | Spawn")
	TSoftClassPtr<ASlotActor> UseableItemSlotActor;
	
	UPROPERTY(EditAnywhere, config, Category = "UseableItem | UI",
		meta = (AllowedClasses = "/Script/UMG.UserWidget"))
	TSoftClassPtr<UUserWidget> ItemHoverWidget;

	UPROPERTY(EditAnywhere, Config, Category = "GridSpawn")
	TSoftClassPtr<class AGridActor> GridActor;

	UPROPERTY(EditAnywhere, Config, Category = "BossData | Tutorial")
	FBossData TutorialBossData;

	UPROPERTY(EditAnywhere, Config, Category = "BossData | Stages")
	TArray<FBossData> AllBossData;

	UPROPERTY(EditAnywhere, config, Category = "Stage UI",
		meta = (AllowedClasses = "/Script/UMG.UserWidget"))
	TSoftClassPtr<UUserWidget> StageHUDWidgetClass;
};
