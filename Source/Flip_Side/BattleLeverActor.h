#pragma once

#include "CoreMinimal.h"
#include "LeverActor.h"
#include "BattleClickInterface.h"
#include "BattleLeverActor.generated.h"

UCLASS()
class FLIP_SIDE_API ABattleLeverActor : public ALeverActor, public IBattleClickInterface
{
    GENERATED_BODY()

protected:
    virtual void ExecuteLeverLogic() override;

public:
    virtual void OnClicked_Implementation() override;
};