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

	// 보스 공격 모션이 끝난 뒤 이 시간(초)만큼 멈췄다가 다음 턴으로 넘어갑니다. 코인이 부서지는 연출이 잘려 보이지 않게 합니다. 0이면 바로 넘어갑니다.
	UPROPERTY(EditAnywhere, config, Category = "Battle | Pacing", meta = (ClampMin = "0.0"))
	float BossAttackEndHoldSeconds = 0.5f;

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

	// 내 코인이 피해를 받고 살아남았을 때(마지막 일격 제외)와 쓰러질 때 소리입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinHitSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinDeathSFX;

	// 내 코인이 회복(ApplyHeal)으로 HP가 실제로 올랐을 때 소리입니다. 여러 코인이 한꺼번에 회복돼도 겹치지 않게 사운드 쪽에서 막습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CoinHealSFX;

	// 보스 차례가 끝나고 내 차례(코인 준비 페이즈)가 돌아올 때 소리입니다. 첫 턴에는 나지 않습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> TurnStartSFX;

	// 카드 조건이 맞아 카드 효과가 코인에 새로 들어갔을 때 소리입니다. 내 차례(코인 행동 페이즈)에만 나고, 여러 장이 한꺼번에 발동해도 한 번만 납니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> CardActivateSFX;

	// 무기 능력·보스 버프로 코인 능력치가 실제로 올랐을 때 소리입니다. 카드·아이템 버프, 디버프, 버프 만료, 다음 턴 복원에는 나지 않습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> StatusBuffGainSFX;

	// 자동 포탑 설치 세트가 칸을 골라 포탑을 실제로 놓는 순간과, 설치된 포탑이 보스를 쏠 때 소리입니다. 무기의 공격 소리는 DB sfx_path에 따로 있습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> TurretDeploySFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | SFX")
	TSoftObjectPtr<USoundBase> TurretFireSFX;

	// 보스 공통 소리입니다. 한 번 맞을 때는 실드 파괴 > 보스 피격 > 실드 피격 중 하나만 납니다. 그로기 보스가 피해 0으로 막을 때도 실드 피격 소리가 납니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> BossPhaseStartSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> BossHitSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> BossShieldHitSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> BossShieldBreakSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> BossDeathSFX;

	// 보스 기믹 소리입니다. 그로기 게이지가 가득 찰 때, 역할 타겟 보스가 표적 역할을 정할 때 납니다. 패턴 공격 소리는 DB boss_pattern_def.sfx_path에 있습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> GroggyBreakSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Boss")
	TSoftObjectPtr<USoundBase> RoleMarkSFX;

	// 스테이지 결과 창이 뜰 때 신호음입니다. 이때 전투 BGM은 서서히 줄어듭니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Stinger")
	TSoftObjectPtr<USoundBase> StageClearSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Stinger")
	TSoftObjectPtr<USoundBase> GameOverSFX;

	// 마지막 보스를 이겨 게임 클리어 창이 뜰 때 신호음입니다. 비워 두면 스테이지 클리어 소리를 씁니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Stinger")
	TSoftObjectPtr<USoundBase> GameClearSFX;

	// 상점 소리입니다. 골드 지불·부족은 모든 구매(코인 슬롯·코인 개수·아이템·카드·무기 해금)에 공통으로 납니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> ShopPageMoveSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> GoldSpendSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> GoldShortSFX;

	// 무기 해금 버튼을 누르고 있는 동안 나는 소리(손을 떼면 멈춤)와 해금이 끝났을 때 소리입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> UnlockHoldSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> UnlockCompleteSFX;

	// 보스를 이기고 돌아온 상점에서 보스 진행 표시의 다음 칸이 열릴 때 소리입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> BossProgressOpenSFX;

	// 보유 카드를 고르거나 내려놓을 때 소리입니다. 카드 칸은 버튼이 아니라 스타일 소리가 없습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> CardSelectSFX;

	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> CardUnselectSFX;

	// 코인 관리에서 3D 코인을 눌러 앞뒷면 무기가 호를 그리며 자리를 바꿀 때 소리입니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | Shop")
	TSoftObjectPtr<USoundBase> CoinSideFlipSFX;

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

	// 튜토리얼 설명을 클릭해 다음으로 넘길 때 소리입니다. 전투 튜토리얼 설명창과 상점 튜토리얼 대화창이 함께 씁니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | UI")
	TSoftObjectPtr<USoundBase> TutorialNextSFX;

	// 상점 튜토리얼 대화창이 숨겨져 있다가 나타날 때 소리입니다. 대사만 바뀔 때는 나지 않습니다.
	UPROPERTY(Config, EditAnywhere, Category = "Sound | UI")
	TSoftObjectPtr<USoundBase> TutorialDialogueOpenSFX;

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
