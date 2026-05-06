#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_RoleTarget.generated.h"

UCLASS()
class FLIP_SIDE_API UBossGimmick_RoleTarget : public UBossGimmickBase
{
    GENERATED_BODY()

public:
    virtual void OnPlayerTurnStart(ABossActor* Boss) override;
    virtual void OnCoinLanded(ABossActor* Boss, FBossTurnContext& Context) override;
    virtual void OnDamageCalculate(ABossActor* Boss, int32& InOutDamage) override;
};
