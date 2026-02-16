#pragma once

#include "CoreMinimal.h"
#include "LeverActor.h"
#include "BattleLeverActor.generated.h"

UCLASS()
class FLIP_SIDE_API ABattleLeverActor : public ALeverActor
{
    GENERATED_BODY()

protected:
    virtual void ExecuteLeverLogic() override;
};