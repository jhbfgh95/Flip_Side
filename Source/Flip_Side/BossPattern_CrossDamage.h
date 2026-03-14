#pragma once

#include "CoreMinimal.h"
#include "BossPatternBase.h"
#include "AttackAreaTypes.h"
#include "BossPattern_CrossDamage.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossPattern_CrossDamage : public UBossPatternBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FGridPoint AnchorCell = FGridPoint{ 3, 2 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	int32 HalfX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	int32 HalfY = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	int32 Damage = 1;

public:
	virtual void BuildTargetCells(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		TArray<FGridPoint>& OutCells) override;

	virtual void ExecutePattern(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		const TArray<FGridPoint>& InLockedCells,
		const TArray<ACoinActor*>& InLockedTargets) override;
};