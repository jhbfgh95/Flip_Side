#pragma once

#include "CoreMinimal.h"
#include "Actors/BossActor.h"
#include "Tutorial_BossActor.generated.h"

/**
  - 튜토리얼 보스
  - 앞으로 이거 보고 보스들 짜시면 됩니다.
 */

#define MAX_TUTO_BOSS_SHIELD 50

UCLASS()
class FLIP_SIDE_API ATutorial_BossActor : public ABossActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Boss | Stat")
	int32 CurrentShield = 50;

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyDamage(int32 Damage, AActor* DamageCauser) override;

	void ApplyShieldHeal(int Heal, AActor* HealCauser);
};
