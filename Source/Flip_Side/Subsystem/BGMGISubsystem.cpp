#include "Subsystem/BGMGISubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UBGMGISubsystem::Deinitialize()
{
    if (IsValid(CurrentBGM))
    {
        CurrentBGM->Stop();
    }
    for (UAudioComponent* FadingBGM : FadingBGMs)
    {
        if (IsValid(FadingBGM))
        {
            FadingBGM->Stop();
        }
    }
    CurrentBGM = nullptr;
    FadingBGMs.Reset();

    Super::Deinitialize();
}

void UBGMGISubsystem::PlayBGM(const UObject* WorldContextObject, USoundBase* Sound, float FadeInSeconds, float FadeOutSeconds)
{
    if (IsValid(CurrentBGM) && CurrentBGM->IsPlaying() && CurrentBGM->Sound == Sound)
    {
        return;
    }

    FadeOutBGM(FadeOutSeconds);
    if (!Sound)
    {
        return;
    }

    // 레벨이 바뀌어도 이어지도록 월드에 묶지 않고 만들고(persist), 참조는 이 서브시스템이 들고 있습니다.
    CurrentBGM = UGameplayStatics::CreateSound2D(WorldContextObject, Sound, 1.f, 1.f, 0.f, nullptr, true, true);
    if (IsValid(CurrentBGM))
    {
        CurrentBGM->FadeIn(FMath::Max(0.f, FadeInSeconds), 1.f, 0.f, EAudioFaderCurve::SCurve);
    }
}

void UBGMGISubsystem::FadeOutBGM(float FadeOutSeconds)
{
    FadingBGMs.RemoveAll([](const TObjectPtr<UAudioComponent>& FadingBGM)
    {
        return !IsValid(FadingBGM) || !FadingBGM->IsPlaying();
    });

    if (IsValid(CurrentBGM) && CurrentBGM->IsPlaying())
    {
        if (FadeOutSeconds > 0.f)
        {
            // 0까지 줄어들면 스스로 멈추고(bAutoDestroy) 사라집니다.
            CurrentBGM->FadeOut(FadeOutSeconds, 0.f, EAudioFaderCurve::SCurve);
            FadingBGMs.Add(CurrentBGM);
        }
        else
        {
            CurrentBGM->Stop();
        }
    }
    CurrentBGM = nullptr;
}
