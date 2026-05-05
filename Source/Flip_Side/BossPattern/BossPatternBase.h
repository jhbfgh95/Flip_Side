#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridTypes.h"
#include "BossDataTypes.h"
#include "BossTurnContext.h"
#include "BossPatternBase.generated.h"

class ABossActor;
class ACoinActor;
class ABase_OtherActor;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossPatternBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FLinearColor TelegraphColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TArray<FBossPatternBattleData> PatternData;

public:
	virtual void BuildTargetCells(
		ABossActor* Boss,
		TArray<FGridPoint>& OutCells, int32 PatternNum);

	virtual void ExecutePattern(
		ABossActor* Boss,
		FBossTurnContext& Context,
		const TArray<ACoinActor*>& InLockedTargets,
		const TArray<ABase_OtherActor*>& InLockedOthers);

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual void ExecuteDamage(const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers, ABossActor* Boss, int32 Damage);

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FLinearColor GetTelegraphColor() const { return TelegraphColor; }

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FString GetPatternName(int32 PatternIndex) const { return PatternData[PatternIndex].PatternName; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Pattern | Effect")
	void PlayPatternEffect(int32 PatternNum, FVector EffectLocation);
	virtual void PlayPatternEffect_Implementation(int32 PatternNum, FVector EffectLocation);
};