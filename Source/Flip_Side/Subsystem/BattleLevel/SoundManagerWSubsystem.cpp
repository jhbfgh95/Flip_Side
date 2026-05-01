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

    if (UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinManager->OnCoinAddedToReady.AddDynamic(this, &USoundManagerWSubsystem::OnCoinAddedToReady);
    }
}

void USoundManagerWSubsystem::OnTurnChanged(ETurnState NewTurn)
{
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!Settings) return;

    if (NewTurn == ETurnState::CoinReadyTurn)
    {
        USoundBase* Sound = Settings->DrawerOpenSFX.LoadSynchronous();
        float Delay = Settings->DrawerOpenSFXDelay;
        if (Delay <= 0.f)
        {
            PlaySFX(Sound);
        }
        else
        {
            GetWorld()->GetTimerManager().SetTimer(DrawerOpenDelayHandle, [this, Sound]()
            {
                PlaySFX(Sound);
            }, Delay, false);
        }
    }
    else if (NewTurn == ETurnState::CoinSelectTurn)
    {
        USoundBase* Sound = Settings->DrawerCloseSFX.LoadSynchronous();
        float Delay = Settings->DrawerCloseSFXDelay;
        if (Delay <= 0.f)
        {
            PlaySFX(Sound);
        }
        else
        {
            GetWorld()->GetTimerManager().SetTimer(DrawerCloseDelayHandle, [this, Sound]()
            {
                PlaySFX(Sound);
            }, Delay, false);
        }
    }
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

void USoundManagerWSubsystem::PlaySFX(USoundBase* Sound)
{
    if (!Sound) return;
    UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}