#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BGMGISubsystem.generated.h"

class UAudioComponent;
class USoundBase;

// 레벨이 바뀌어도 BGM이 뚝 끊기지 않도록, 지금 재생 중인 BGM을 게임 인스턴스에서 들고 있습니다.
// 어떤 곡을 틀지는 각 레벨의 사운드 관리자(USoundManagerWSubsystem)가 정합니다.
UCLASS()
class FLIP_SIDE_API UBGMGISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    // 지금 곡과 같으면 그대로 이어서 틀고, 다르면 지금 곡은 서서히 줄이고 새 곡은 서서히 키웁니다.
    void PlayBGM(const UObject* WorldContextObject, USoundBase* Sound, float FadeInSeconds, float FadeOutSeconds);

    // 지금 곡을 서서히 멈춥니다. 인트로 영상처럼 BGM이 없어야 할 때 씁니다.
    void FadeOutBGM(float FadeOutSeconds);

private:
    UPROPERTY()
    TObjectPtr<UAudioComponent> CurrentBGM;

    // 줄어드는 중인 이전 곡입니다. 다 줄어들 때까지 참조를 들고 있어야 중간에 끊기지 않습니다.
    UPROPERTY()
    TArray<TObjectPtr<UAudioComponent>> FadingBGMs;
};
