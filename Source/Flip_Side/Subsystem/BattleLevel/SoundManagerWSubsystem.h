#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Components/SlateWrapperTypes.h"
#include "FlipSide_Enum.h"
#include "SoundManagerWSubsystem.generated.h"

class UUserWidget;
class UWidget;
struct FPointerEvent;

UCLASS()
class FLIP_SIDE_API USoundManagerWSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

protected:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    // 레벨이 열린 직후의 긴 프레임이 지나간 뒤 BGM을 시작하기 위한 타이머입니다.
    FTimerHandle BGMStartTimer;

    // PollCursorMoved가 마지막으로 본 커서 위치입니다.
    FVector2D LastPolledCursorPos = FVector2D::ZeroVector;
    bool bHasPolledCursor = false;

	UFUNCTION()
    void OnPhaseChanged(EPhaseState NewPhase);

    UFUNCTION()
    void OnCoinAddedToReady();

    void PlaySFX(USoundBase* Sound);

    // 지금 맵에 맞는 BGM을 고릅니다. 정해진 BGM이 없으면 nullptr입니다.
    USoundBase* ChooseLevelBGM() const;

public:
    // 지금 맵의 BGM을 재생합니다. 같은 곡이 이미 나오고 있으면 그대로 이어지고, 다른 곡이면 서로 겹치며 바뀝니다.
    void PlayLevelBGM();

    // 지금 BGM을 서서히 멈춥니다. 시작 화면에서 인트로 영상을 틀기 전과 스테이지 결과 창이 뜰 때 부릅니다.
    void FadeOutLevelBGM();

    void PlayLeverPullSound();
    void PlayLeverReturnSound();

    // 공통 UI 소리입니다. 팝업을 열고 닫거나 경고가 뜰 때 위젯에서 부릅니다.
    void PlayPopupOpenSound();
    void PlayPopupCloseSound();
    void PlayWarningSound();

    // 튜토리얼 설명을 넘길 때(전투 설명창 클릭, 상점 대화 클릭)와 상점 튜토리얼 대화창이 나타날 때 소리입니다.
    void PlayTutorialNextSound();
    void PlayTutorialDialogueOpenSound();

    // 팝업을 보이거나 숨기면서, 실제로 바뀔 때만 열기/닫기 소리를 냅니다. bSilent면 소리 없이 바꿉니다.
    static void SetPopupVisibility(UWidget* Popup, ESlateVisibility NewVisibility, bool bSilent = false);

    // 버튼이 아닌 것에 마우스를 올릴 때 소리입니다. 버튼은 스타일에 같은 소리가 들어 있습니다.
    void PlayHoverSound();

    // 지난번 확인 뒤 커서가 움직였는지 돌려줍니다. 컨트롤러가 매 틱 부르고, 움직였을 때만 3D 오브젝트 호버 소리를 냅니다.
    bool PollCursorMoved();

    // 커서 아래에 버튼이 있는지 봅니다. 버튼 소리가 이미 나므로 그 뒤의 3D 오브젝트 소리는 내지 않습니다.
    static bool IsSlateButtonUnderCursor();

    // 위젯의 NativeOnMouseEnter에서 부릅니다. 위젯이 생기거나 사라져서 생긴 가짜 이동에는 소리를 내지 않습니다.
    static void PlayHoverSoundFromWidget(const UUserWidget* Widget, const FPointerEvent& MouseEvent);

    void PlayCoinActionSound(USoundBase* Sound);
    void PlayCoinActionFailedSound();
    void PlayCoinLaunchSound();
    void PlayCoinTeleportSound();
    void PlayCoinClickSound();

    // 내 코인이 맞았을 때(살아남은 피격)와 쓰러질 때 소리입니다. 여러 코인이 한꺼번에 맞아도 겹치지 않게 사운드 쪽에서 막습니다.
    void PlayCoinHitSound();
    void PlayCoinDeathSound();

    // 내 코인이 회복될 때 소리입니다. 여러 코인이 한꺼번에 회복돼도 한 번만 나게 사운드 쪽에서 막습니다.
    void PlayCoinHealSound();

    // 카드 효과가 코인에 새로 들어갔을 때와 무기 능력으로 코인 능력치가 올랐을 때 소리입니다. 한꺼번에 여러 번 불려도 사운드 쪽에서 한 번만 나게 막습니다.
    void PlayCardActivateSound();
    void PlayStatusBuffGainSound();

    // 자동 포탑을 실제로 설치한 순간과 설치된 포탑이 보스를 쏠 때 소리입니다.
    void PlayTurretDeploySound();
    void PlayTurretFireSound();

    // 물약을 쓰는 데 성공했을 때 소리입니다. 소리는 물약마다 DB(item.sfx_path)에 있습니다. 쓸 수 없는 곳에 누르면 PlayCoinActionFailedSound를 씁니다.
    void PlayItemUseSound(USoundBase* Sound);

    // 보스 소리입니다. 보스 5종이 함께 씁니다.
    void PlayBossPhaseStartSound();
    void PlayBossHitSound();
    void PlayBossShieldHitSound();
    void PlayBossShieldBreakSound();
    void PlayBossDeathSound();

    // 보스 패턴 공격 소리입니다. 소리는 패턴마다 DB(boss_pattern_def.sfx_path)에 있고, BossVFX 노티파이에서 부릅니다.
    void PlayBossPatternSound(USoundBase* Sound);
    // 그로기 게이지가 가득 찼을 때, 역할 타겟 보스가 표적 역할을 정했을 때 소리입니다.
    void PlayGroggyBreakSound();
    void PlayRoleMarkSound();

    // 스테이지 결과 창이 뜰 때 신호음입니다.
    void PlayStageClearSound();
    void PlayGameOverSound();

    // 게임 클리어 창이 뜰 때 신호음입니다. 길어서 시작 화면으로 넘어가도 끊기지 않게 BGM처럼 들고 갑니다. GameClearSFX가 비어 있으면 스테이지 클리어 소리를 냅니다.
    void PlayGameClearSound();

    // 상점 소리입니다. 골드 지불·부족은 MoneyGISubsystem::SpendMoney에서 냅니다(아이템 골드 부족만 UShopItemPresenter::RequestPurchaseFromPopup에서 냅니다).
    void PlayShopPageMoveSound();
    void PlayGoldSpendSound();
    void PlayGoldShortSound();
    void PlayUnlockCompleteSound();

    // 상점 보스 진행 칸이 열리는 애니메이션과 함께 냅니다(W_ShopBossProgressSlot::OpenSlot).
    void PlayBossProgressOpenSound();

    // 보유 카드를 고를 때와 내려놓을 때 소리입니다. UShopCardPresenter에서 실제로 바뀐 경우에만 부릅니다.
    void PlayCardSelectSound();
    void PlayCardUnselectSound();

    // 코인 관리에서 코인 앞뒷면이 자리를 바꿀 때 소리입니다(UShopCoinPresenter::ChangeCoinSide).
    void PlayCoinSideFlipSound();

    // 무기 해금 버튼을 누르고 있는 동안만 나는 소리입니다. 손을 떼거나 해금이 끝나면 멈춥니다.
    void StartUnlockHoldSound();
    void StopUnlockHoldSound();

private:
    UPROPERTY()
    TObjectPtr<class UAudioComponent> UnlockHoldAudio;
};
