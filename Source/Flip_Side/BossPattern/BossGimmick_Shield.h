#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Shield.generated.h"

UCLASS()
class FLIP_SIDE_API UBossGimmick_Shield : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	virtual void OnBattleStart(ABossActor* Boss) override;
};