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
class UW_StageEnd;
class UW_MoneyDisplay;
class UW_BattlePhaseAndTurnDisplayUI;
class UW_BattleTutorialOverlay;
class UBattleTutorialSequenceData;
class AUseableItemActor;
class UNiagaraSystem;
class USoundBase;
class UW_PoisonTimerUI;

UCLASS(Config=Game, meta=(DisplayName="My Manager Settings"))
class FLIP_SIDE_API UFlipSideDevloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Common | UI")
	TSoftClassPtr<UUserWidget> CursorWidgetClass;

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

	UPROPERTY(EditAnywhere, config, Category = "Battle | UI")
	TSoftClassPtr<UW_StageEnd> StageEndWidgetClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | UI")
	TSoftClassPtr<UW_BattlePhaseAndTurnDisplayUI> BattlePhaseAndTurnDisplayWidgetClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | UI")
	TSoftClassPtr<UW_PoisonTimerUI> PoisonTimerWidgetClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | Tutorial")
	TSoftObjectPtr<UBattleTutorialSequenceData> BattleTutorialSequenceData;

	UPROPERTY(EditAnywhere, config, Category = "Battle | Tutorial")
	TSoftClassPtr<UW_BattleTutorialOverlay> BattleTutorialOverlayWidgetClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | Tutorial")
	float BattleTutorialInitDelay = 0.2f;

	UPROPERTY(EditAnywhere, config, Category = "Common | UI")
	TSoftClassPtr<UW_MoneyDisplay> MoneyDisplayWidgetClass;

	// 테스트용 — 0이면 기본 폴백(BossID=1), 1 이상이면 해당 ID로 강제 스폰
	UPROPERTY(Config, EditAnywhere, Category = "Debug | Boss")
	int32 DebugForceBossID = 0;

	// 사운드
	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> BattleBGM;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> ShopBGM;

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

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinActionFailedSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinTeleportSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinClickSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> ShopBuyClickSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> ShopSellClickSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> BattleItemClickSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> DefaultClickSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Battle VFX")
	TSoftObjectPtr<UNiagaraSystem> Boss_Hit_VFX;

	UPROPERTY(Config, EditAnywhere, Category = "Battle VFX")
	TSoftObjectPtr<UNiagaraSystem> Boss_CC_VFX;

	UPROPERTY(Config, EditAnywhere, Category = "Battle VFX")
	TSoftObjectPtr<UNiagaraSystem> Coin_Logic_Failed_VFX;

	UPROPERTY(Config, EditAnywhere, Category = "Battle VFX")
	TSoftObjectPtr<UNiagaraSystem> Promotion_VFX;

	UPROPERTY(Config, EditAnywhere, Category = "Battle VFX")
	float CommonVFXDelayAfterCoinVFX = 0.35f;
};
