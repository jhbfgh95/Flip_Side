#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_RoleTarget.generated.h"

UCLASS()
class FLIP_SIDE_API UBossGimmick_RoleTarget : public UBossGimmickBase
{
    GENERATED_BODY()

public:
    virtual void OnDamageCalculate(ABossActor* Boss, int32& InOutDamage) override;
};
