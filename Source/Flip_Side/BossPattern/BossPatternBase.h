#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "BossPatternBase.generated.h"

class UBossManagerSubsystem;
class ABossActor;
class ACoinActor;

USTRUCT(BlueprintType)
struct FPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FString PatternName = TEXT("Default Pattern");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FAttackAreaSpec PatternSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	int32 Damage = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	float AttackApplyDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	class UAnimMontage* PatternMontage;

	//class UNaiagara...음;
};

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossPatternBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FLinearColor TelegraphColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	float TelegraphDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TArray<FPatternData> PatternData;

public:
	virtual void BuildTargetCells(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		TArray<FGridPoint>& OutCells, int32 PatternNum);

	virtual void ExecutePattern(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		const TArray<FGridPoint>& InLockedCells,
		const TArray<ACoinActor*>& InLockedTargets, int32 PatternNum);

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual float GetTelegraphDuration() const { return TelegraphDuration; }

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FLinearColor GetTelegraphColor() const { return TelegraphColor; }

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FString GetPatternName(int32 PatternIndex) const { return PatternData[PatternIndex].PatternName; }
};