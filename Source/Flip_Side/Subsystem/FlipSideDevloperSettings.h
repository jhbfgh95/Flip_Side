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
class UW_ItemInfo;
class UUserWidget;
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
	
	UPROPERTY(EditAnywhere, config, Category = "UseableItem | UI")
	TSoftClassPtr<UW_ItemInfo> ItemHoverWidget;

	UPROPERTY(EditAnywhere, Config, Category = "GridSpawn")
	TSoftClassPtr<class AGridActor> GridActor;

	UPROPERTY(EditAnywhere, config, Category = "Stage UI",
		meta = (AllowedClasses = "/Script/UMG.UserWidget"))
	TSoftClassPtr<UUserWidget> StageHUDWidgetClass;

	// 사운드
	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> BattleBGM;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> LeverPullSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> DrawerOpenSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	float DrawerOpenSFXDelay = 0.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> DrawerCloseSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	float DrawerCloseSFXDelay = 0.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinSlotClickSFX;
};
