#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Groggy.generated.h"

UCLASS()
class FLIP_SIDE_API UBossGimmick_Groggy : public UBossGimmickBase
{
    GENERATED_BODY()

public:
    virtual void OnBattleStart(ABossActor* Boss) override;
    virtual void OnDamageCalculate(ABossActor* Boss, int32& InOutDamage) override;

    void AddGroggyValue(int32 Amount, ABossActor* Boss);

    bool IsGroggy() const { return bGroggy; }
    int32 GetCurrentGroggy() const { return CurrentGroggy; }
    int32 GetMaxGroggy() const { return MaxGroggy; }

private:
    int32 MaxGroggy = 0;
    int32 CurrentGroggy = 0;
    bool bGroggy = false;
};
