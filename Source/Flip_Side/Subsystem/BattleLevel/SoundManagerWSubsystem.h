#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlipSide_Enum.h"
#include "SoundManagerWSubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API USoundManagerWSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

protected:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    UPROPERTY()
    class UAudioComponent* BGMComponent = nullptr;

	UFUNCTION()
    void OnPhaseChanged(EPhaseState NewPhase);

    UFUNCTION()
    void OnCoinAddedToReady();

    void PlaySFX(USoundBase* Sound);

public:
    void PlayLeverPullSound();
    void PlayCoinActionSound(USoundBase* Sound);
    void PlayCoinActionFailedSound();
    void PlayCoinTeleportSound();
    void PlayCoinClickSound();
};
