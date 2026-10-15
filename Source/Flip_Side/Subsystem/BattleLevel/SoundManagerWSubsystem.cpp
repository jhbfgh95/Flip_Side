#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystem/BGMGISubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/GameSettingGISubsystem.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/Events.h"
#include "Layout/WidgetPath.h"
#include "TimerManager.h"

bool USoundManagerWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    UWorld* World = Cast<UWorld>(Outer);
    if (World)
    {
        FString MapName = World->GetName();
        // 전투 맵뿐 아니라 시작 메뉴와 상점(상점 튜토리얼 포함) 맵에서도 BGM과 볼륨을 다룹니다.
        if (MapName.Contains(TEXT("L_Stage")) || MapName.Contains(TEXT("L_GameStart")) || MapName.Contains(TEXT("Shop")))
            return true;
    }
    return false;
}

void USoundManagerWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    // 설정 화면에서 저장한 전체 볼륨을 레벨이 열릴 때마다 다시 적용합니다.
    if (UGameInstance* GameInstance = InWorld.GetGameInstance())
    {
        if (const UGameSettingGISubsystem* GameSettings = GameInstance->GetSubsystem<UGameSettingGISubsystem>())
        {
            GameSettings->ApplyMasterVolume(&InWorld);
        }
    }

    // 시작 메뉴는 인트로 영상이 끝난 뒤 W_GameStartWidget이 PlayLevelBGM을 부릅니다.
    if (!InWorld.GetName().Contains(TEXT("L_GameStart")))
    {
        PlayLevelBGM();
    }

    if (UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>())
    {
        BattleManager->OnPhaseChanged.AddDynamic(this, &USoundManagerWSubsystem::OnPhaseChanged);
    }

    // 코인을 준비 칸에 넣을 때마다 CoinSlotClickSFX를 재생합니다.
    if (UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinManager->OnCoinAddedToReady.AddDynamic(this, &USoundManagerWSubsystem::OnCoinAddedToReady);
    }
}

void USoundManagerWSubsystem::PlayLevelBGM()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 레벨을 불러온 직후 한 프레임은 시간이 길게 잡혀 페이드 앞부분이 건너뛰므로, 레벨이 열리고 조금 뒤에 시작합니다.
    constexpr float BGMStartDelaySeconds = 0.5f;
    if (World->GetTimeSeconds() < BGMStartDelaySeconds)
    {
        if (!World->GetTimerManager().IsTimerActive(BGMStartTimer))
        {
            World->GetTimerManager().SetTimer(BGMStartTimer, this, &USoundManagerWSubsystem::PlayLevelBGM,
                BGMStartDelaySeconds - World->GetTimeSeconds(), false);
        }
        return;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    UBGMGISubsystem* BGMSubsystem = GameInstance ? GameInstance->GetSubsystem<UBGMGISubsystem>() : nullptr;
    if (!BGMSubsystem) return;

    // 같은 곡이면 그대로 이어지고, 다른 곡이면 이전 곡은 줄고 새 곡은 서서히 커집니다.
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    BGMSubsystem->PlayBGM(this, ChooseLevelBGM(),
        Settings ? Settings->BGMFadeInSeconds : 0.f, Settings ? Settings->BGMFadeOutSeconds : 0.f);
}

void USoundManagerWSubsystem::FadeOutLevelBGM()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (UBGMGISubsystem* BGMSubsystem = GameInstance ? GameInstance->GetSubsystem<UBGMGISubsystem>() : nullptr)
    {
        const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
        BGMSubsystem->FadeOutBGM(Settings ? Settings->BGMFadeOutSeconds : 0.f);
    }
}

USoundBase* USoundManagerWSubsystem::ChooseLevelBGM() const
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    const UWorld* World = GetWorld();
    if (!Settings || !World) return nullptr;

    const FString MapName = World->GetName();
    const TSoftObjectPtr<USoundBase>* LevelBGM = &Settings->BattleBGM;
    bool bIsTutorial = false;
    if (MapName.Contains(TEXT("L_GameStart")))
    {
        LevelBGM = &Settings->MainMenuBGM;
    }
    else if (MapName.Contains(TEXT("Shop")))
    {
        LevelBGM = &Settings->ShopBGM;
        bIsTutorial = MapName.Contains(TEXT("Tuto"));
    }
    else if (const UGameInstance* GameInstance = World->GetGameInstance())
    {
        // 같은 튜토리얼 맵이라도 튜토리얼 보스전은 일반 전투 BGM을 씁니다.
        const ULevelGISubsystem* LevelGI = GameInstance->GetSubsystem<ULevelGISubsystem>();
        bIsTutorial = IsValid(LevelGI) && LevelGI->IsBattleTutorialActive();
    }

    // 튜토리얼 BGM이 정해져 있으면 튜토리얼 중에는 그것을, 아니면 맵의 기본 BGM을 씁니다.
    if (bIsTutorial && !Settings->TutorialBGM.IsNull())
    {
        LevelBGM = &Settings->TutorialBGM;
    }
    return LevelBGM->IsNull() ? nullptr : LevelBGM->LoadSynchronous();
}

void USoundManagerWSubsystem::OnPhaseChanged(EPhaseState NewPhase)
{
    // 보스 차례가 끝나고 내 차례가 돌아올 때 턴 시작 소리를 냅니다. 첫 턴은 전투 시작 연출과 겹치므로 내지 않습니다.
    if (NewPhase != EPhaseState::CoinReadyPhase) return;

    UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (!IsValid(BattleManager) || BattleManager->GetTurnCount() <= 1) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->TurnStartSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::OnCoinAddedToReady()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinSlotClickSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayLeverPullSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->LeverPullSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayLeverReturnSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->LeverReturnSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayPopupOpenSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->PopupOpenSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayPopupCloseSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->PopupCloseSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayWarningSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->WarningSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayHoverSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->HoverSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::SetPopupVisibility(UWidget* Popup, ESlateVisibility NewVisibility, bool bSilent)
{
    if (!IsValid(Popup)) return;

    const bool bWasVisible = Popup->IsVisible();
    Popup->SetVisibility(NewVisibility);
    const bool bIsVisible = Popup->IsVisible();
    if (bSilent || bWasVisible == bIsVisible) return;

    if (UWorld* World = Popup->GetWorld())
    {
        if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
        {
            if (bIsVisible)
            {
                SoundManager->PlayPopupOpenSound();
            }
            else
            {
                SoundManager->PlayPopupCloseSound();
            }
        }
    }
}

bool USoundManagerWSubsystem::PollCursorMoved()
{
    if (!FSlateApplication::IsInitialized()) return false;

    const FVector2D CursorPos(FSlateApplication::Get().GetCursorPos());
    const bool bMoved = bHasPolledCursor && !CursorPos.Equals(LastPolledCursorPos, 0.5f);
    LastPolledCursorPos = CursorPos;
    bHasPolledCursor = true;
    return bMoved;
}

bool USoundManagerWSubsystem::IsSlateButtonUnderCursor()
{
    if (!FSlateApplication::IsInitialized()) return false;

    FSlateApplication& App = FSlateApplication::Get();
    const FWidgetPath Path = App.LocateWindowUnderMouse(App.GetCursorPos(), App.GetInteractiveTopLevelWindows());
    for (int32 Index = 0; Index < Path.Widgets.Num(); ++Index)
    {
        const FName Type = Path.Widgets[Index].Widget->GetType();
        if (Type == TEXT("SButton") || Type == TEXT("SComboButton")) return true;
    }
    return false;
}

void USoundManagerWSubsystem::PlayHoverSoundFromWidget(const UUserWidget* Widget, const FPointerEvent& MouseEvent)
{
    // 커서는 그대로인데 위젯이 생기거나 움직여서 생긴 진입은 커서 이동량이 0이므로 건너뜁니다.
    if (!IsValid(Widget) || MouseEvent.GetCursorDelta().IsNearlyZero()) return;

    if (UWorld* World = Widget->GetWorld())
    {
        if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayHoverSound();
        }
    }
}

void USoundManagerWSubsystem::PlayCoinActionSound(USoundBase* Sound)
{
    PlaySFX(Sound);
}

void USoundManagerWSubsystem::PlayCoinActionFailedSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinActionFailedSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayCoinLaunchSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinLaunchSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayCoinTeleportSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinTeleportSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayCoinClickSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinClickSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayCoinHitSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinHitSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayCoinDeathSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinDeathSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayCoinHealSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->CoinHealSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayItemUseSound(USoundBase* Sound)
{
    PlaySFX(Sound);
}

void USoundManagerWSubsystem::PlayBossPhaseStartSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->BossPhaseStartSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayBossHitSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->BossHitSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayBossShieldHitSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->BossShieldHitSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayBossShieldBreakSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->BossShieldBreakSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayBossDeathSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->BossDeathSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayBossPatternSound(USoundBase* Sound)
{
    PlaySFX(Sound);
}

void USoundManagerWSubsystem::PlayGroggyBreakSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->GroggyBreakSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayRoleMarkSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->RoleMarkSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayStageClearSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->StageClearSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayGameOverSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->GameOverSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayShopPageMoveSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->ShopPageMoveSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayGoldSpendSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->GoldSpendSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayGoldShortSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->GoldShortSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::PlayUnlockCompleteSound()
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;
    PlaySFX(Settings->UnlockCompleteSFX.LoadSynchronous());
}

void USoundManagerWSubsystem::StartUnlockHoldSound()
{
    StopUnlockHoldSound();

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    USoundBase* Sound = Settings ? Settings->UnlockHoldSFX.LoadSynchronous() : nullptr;
    if (!Sound) return;
    // 끝나면 스스로 사라지는 컴포넌트입니다. 멈출 때를 위해 참조만 들고 있습니다.
    UnlockHoldAudio = UGameplayStatics::SpawnSound2D(GetWorld(), Sound);
}

void USoundManagerWSubsystem::StopUnlockHoldSound()
{
    if (IsValid(UnlockHoldAudio))
    {
        UnlockHoldAudio->FadeOut(0.08f, 0.f);
    }
    UnlockHoldAudio = nullptr;
}

void USoundManagerWSubsystem::PlaySFX(USoundBase* Sound)
{
    if (!Sound) return;
    UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}
