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

    // 지금 BGM을 서서히 멈춥니다. 시작 화면에서 인트로 영상을 틀기 전에 부릅니다.
    void FadeOutLevelBGM();

    void PlayLeverPullSound();
    void PlayLeverReturnSound();

    // 공통 UI 소리입니다. 팝업을 열고 닫거나 경고가 뜰 때 위젯에서 부릅니다.
    void PlayPopupOpenSound();
    void PlayPopupCloseSound();
    void PlayWarningSound();

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
};
