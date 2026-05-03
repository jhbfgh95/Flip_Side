#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Proxy.generated.h"

// ActiveGimmick으로 세팅되는 래퍼.
// OnDamageCalculate → 선택된 기믹에만 전달
// OnPatternExecute  → 선택된 기믹에만 전달
// OnTurnEnd         → 선택된 기믹에만 전달
// OnPlayerTurnStart / OnPlayerTurnEnd → GimmickList 전체에 브로드캐스트
UCLASS()
class FLIP_SIDE_API UBossGimmick_Proxy : public UBossGimmickBase
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TObjectPtr<UBossGimmickBase> SelectedGimmick;

    virtual void OnDamageCalculate(ABossActor* Boss, int32& InOutDamage) override
    {
        if (IsValid(SelectedGimmick))
            SelectedGimmick->OnDamageCalculate(Boss, InOutDamage);
    }

    virtual void OnPatternExecute(ABossActor* Boss,
        const TArray<FGridPoint>& LockedCells,
        const TArray<ACoinActor*>& LockedTargets,
        const TArray<ABase_OtherActor*>& LockedOthers) override
    {
        if (IsValid(SelectedGimmick))
            SelectedGimmick->OnPatternExecute(Boss, LockedCells, LockedTargets, LockedOthers);
    }

    virtual void OnTurnEnd(ABossActor* Boss) override
    {
        if (IsValid(SelectedGimmick))
            SelectedGimmick->OnTurnEnd(Boss);
    }

    virtual void OnPlayerTurnStart(ABossActor* Boss) override;
    virtual void OnPlayerTurnEnd(ABossActor* Boss) override;
};
