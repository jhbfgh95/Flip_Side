#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

bool USoundManagerWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    UWorld* World = Cast<UWorld>(Outer);
    if (World)
    {
        FString MapName = World->GetName();
        if (MapName.Contains(TEXT("L_Stage")))
            return true;
    }
    return false;
}

void USoundManagerWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;

    if (!Settings->BattleBGM.IsNull())
    {
        if (USoundBase* BGM = Settings->BattleBGM.LoadSynchronous())
        {
            BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BGM, 1.f, 1.f, 0.f, nullptr, true);
        }
    }

    if (UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>())
    {
        BattleManager->OnTurnChanged.AddDynamic(this, &USoundManagerWSubsystem::OnTurnChanged);
    }

    // TODO: CoinReadyPhase UI 사운드 정책이 확정되면 다시 연결합니다.
    // if (UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>())
    // {
    //     CoinManager->OnCoinAddedToReady.AddDynamic(this, &USoundManagerWSubsystem::OnCoinAddedToReady);
    // }
}

void USoundManagerWSubsystem::OnTurnChanged(ETurnState NewTurn)
{
    // TODO: ReadyCoinWidget 전용 사운드 연출이 확정되면 구현합니다.
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

void USoundManagerWSubsystem::PlaySFX(USoundBase* Sound)
{
    if (!Sound) return;
    UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}
