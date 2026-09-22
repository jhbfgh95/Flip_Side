#pragma once

#include "CoreMinimal.h"
#include "BossActor.h"
#include "BossActor_Groggy.generated.h"

UCLASS()
class FLIP_SIDE_API ABossActor_Groggy : public ABossActor
{
    GENERATED_BODY()

public:
    ABossActor_Groggy();

    void UpdateGroggyBar(int32 CurrentGroggy);
    virtual FBossHUDData GetBossHUDData() const override;

protected:
    virtual void BeginPlay() override;
    virtual void PlayHitAnimation() override;

public:
    virtual bool TryConsumeIncomingCC(const FStatusEffectInstance& Effect) override;
    virtual int32 ApplyDamageAndReturnHPDamage(int32 Damage, AActor* DamageCauser) override;
};
