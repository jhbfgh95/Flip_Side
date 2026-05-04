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

protected:
    virtual void BeginPlay() override;

public:
    virtual void ApplyCC(const FCCStructure& CC) override;
    virtual int32 ApplyDamageAndReturnHPDamage(int32 Damage, AActor* DamageCauser) override;
};
