// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BossDataTypes.h"
#include "FlipSideDevloperSettings.generated.h"

class ACoinActor;
class UBattleCoinInfoWidget;
class UUserWidget;
class UW_StageEnd;
class UW_MoneyDisplay;
class UW_BattleTutorialOverlay;
class UBattleTutorialSequenceData;
class AUseableItemActor;
class UNiagaraSystem;
class ABossCoinActor;

UCLASS(Config=Game, meta=(DisplayName="My Manager Settings"))
class FLIP_SIDE_API UFlipSideDevloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Common | UI")
	TSoftClassPtr<UUserWidget> CursorWidgetClass;

	UPROPERTY(Config, EditAnywhere, Category = "Coin | Spawn")
	TSoftClassPtr<ACoinActor> CoinActor;

	UPROPERTY(EditAnywhere, config, Category = "Coin | UI")
	TSoftClassPtr<UBattleCoinInfoWidget> BattleCoinInfoWidget;

	UPROPERTY(Config, EditAnywhere, Category = "UseableItem | Preview")
	TSoftClassPtr<AUseableItemActor> UseableItemActor;

	UPROPERTY(EditAnywhere, Config, Category = "GridSpawn")
	TSoftClassPtr<class AGridActor> GridActor;

	UPROPERTY(EditAnywhere, Config, Category = "GridSpawn")
	TSoftClassPtr<class ABossWallActor> BossWallActorClass;

	UPROPERTY(EditAnywhere, config, Category = "Setting | UI")
	TSoftClassPtr<UUserWidget> SettingWidget;

	// 항상 그리드 가장 뒤 가운데 3x3 자리에 고정 스폰됨
	UPROPERTY(EditAnywhere, Config, Category = "GridSpawn")
	TSoftClassPtr<ABossCoinActor> BossCoinActorClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | UI")
	TSoftClassPtr<UW_StageEnd> StageEndWidgetClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | Tutorial")
	TSoftObjectPtr<UBattleTutorialSequenceData> BattleTutorialSequenceData;

	UPROPERTY(EditAnywhere, config, Category = "Battle | Tutorial")
	TSoftClassPtr<UW_BattleTutorialOverlay> BattleTutorialOverlayWidgetClass;

	UPROPERTY(EditAnywhere, config, Category = "Battle | Tutorial")
	float BattleTutorialInitDelay = 0.2f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, Category = "Debug | Tutorial", meta = (DisplayName = "PIE에서 전투 튜토리얼 바로 시작", ToolTip = "L_Stage_BattleTutorial을 직접 PIE로 실행하면 영상 없이 설명 모드와 튜토리얼 보스를 준비합니다. PIE를 시작한 맵에만 적용됩니다."))
	bool bStartBattleTutorialInPIE = false;
#endif

	UPROPERTY(EditAnywhere, config, Category = "Common | UI")
	TSoftClassPtr<UW_MoneyDisplay> MoneyDisplayWidgetClass;

	// 테스트용 — 0이면 기본 폴백(BossID=1), 1 이상이면 해당 ID로 강제 스폰
	UPROPERTY(Config, EditAnywhere, Category = "Debug | Boss")
	int32 DebugForceBossID = 0;

	// 사운드
	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> BattleBGM;

	// 맵별 BGM입니다. 비워 두면 시작 메뉴·상점은 BGM 없이, 튜토리얼은 그 맵의 기본 BGM으로 재생합니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> MainMenuBGM;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> ShopBGM;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM")
	TSoftObjectPtr<USoundBase> TutorialBGM;

	// 레벨이 바뀌어 BGM이 새로 시작될 때 서서히 커지는 시간(초)입니다. 0이면 바로 시작합니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM", meta = (ClampMin = "0.0"))
	float BGMFadeInSeconds = 2.0f;

	// 다른 BGM으로 바뀔 때 이전 곡이 서서히 줄어드는 시간(초)입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | BGM", meta = (ClampMin = "0.0"))
	float BGMFadeOutSeconds = 1.0f;

	// 전체 볼륨용 사운드 믹스와 사운드 클래스입니다. 설정의 볼륨 슬라이더가 이 믹스로 클래스(와 하위 클래스)의 소리 크기를 조절합니다.
	// 클래스를 지정하지 않은 소리는 모두 프로젝트 기본 사운드 클래스(엔진 Master)에 속하므로 기본값은 그 클래스입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Volume")
	TSoftObjectPtr<class USoundMix> MasterSoundMix;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Volume")
	TSoftObjectPtr<class USoundClass> MasterSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> LeverPullSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinSlotClickSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinActionFailedSFX;

	// 레버를 당긴 뒤 코인들이 튀어 오를 때 소리입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinLaunchSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinTeleportSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinClickSFX;

	// 레버 UI가 원래 자리로 올라올 때 소리입니다. 비워 두면 소리가 나지 않습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> LeverReturnSFX;

	// 공통 UI 소리입니다. 버튼의 마우스 올리기·클릭 소리는 각 버튼 스타일에 들어 있고, 여기는 코드에서 재생하는 소리입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | UI")
	TSoftObjectPtr<USoundBase> PopupOpenSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | UI")
	TSoftObjectPtr<USoundBase> PopupCloseSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | UI")
	TSoftObjectPtr<USoundBase> WarningSFX;

	// 버튼이 아닌 것(전투 코인·포탑, 상점 3D 오브젝트, 상점 목록 칸)에 마우스를 올릴 때 소리입니다. 버튼 스타일과 같은 소리를 씁니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | UI")
	TSoftObjectPtr<USoundBase> HoverSFX;

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
