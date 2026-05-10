#pragma once

#include "CoreMinimal.h"
#include "LeverActor.h"
#include "BattleClickInterface.h"
#include "BattleHoverInterface.h"
#include "BattleLeverActor.generated.h"

UCLASS()
class FLIP_SIDE_API ABattleLeverActor : public ALeverActor, public IBattleClickInterface, public IBattleHoverInterface
{
    GENERATED_BODY()

protected:
    virtual void ExecuteLeverLogic() override;

    bool bCanAnimate = false;
public:
    virtual void OnClicked_Implementation() override;

    virtual void OnHover_Implementation() override;
    virtual void OnUnhover_Implementation() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Lever | Outline")
    void LeverHoverOutline();

    UFUNCTION(BlueprintImplementableEvent, Category = "Lever | Outline")
    void LeverUnHoverOutline();

    float GetLeverAnimationTotalTime() const
    {
        return FMath::Max(ReturnDelay, AnimationSpeed) + AnimationSpeed;
    }
};