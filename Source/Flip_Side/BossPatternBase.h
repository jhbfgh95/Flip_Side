#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridTypes.h"
#include "BossPatternBase.generated.h"

class UBossManagerSubsystem;
class ABossActor;
class ACoinActor;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossPatternBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FString PatternName = TEXT("PatternBase");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FLinearColor TelegraphColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	float TelegraphDuration = 1.0f;

public:
	virtual void BuildTargetCells(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		TArray<FGridPoint>& OutCells);

	virtual void ExecutePattern(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		const TArray<FGridPoint>& InLockedCells,
		const TArray<ACoinActor*>& InLockedTargets);

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual float GetTelegraphDuration() const { return TelegraphDuration; }

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FLinearColor GetTelegraphColor() const { return TelegraphColor; }

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FString GetPatternName() const { return PatternName; }
};