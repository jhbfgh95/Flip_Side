#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "BossPatternBase.generated.h"

//class UBossManagerSubsystem;
class ABossActor;
class ACoinActor;
class ABase_OtherActor;

USTRUCT(BlueprintType)
struct FPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FString PatternName = TEXT("Default Pattern");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | UI")
	FText PatternDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | UI")
	TObjectPtr<class UTexture2D> PatternIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | UI")
	TObjectPtr<class UTexture2D> PatternRangeImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FAttackAreaSpec PatternSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	int32 Damage = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	float AttackApplyDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TObjectPtr<class UAnimMontage> PatternMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | Visual")
    TSoftObjectPtr<class UNiagaraSystem> PatternEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | Visual")
    FVector PatternScale;

	//투사체 등의 비쥬얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | Visual")
	TSoftClassPtr<class ABase_PatternVisualActor> VisualActorClass;
};

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossPatternBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FLinearColor TelegraphColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TArray<FPatternData> PatternData;

public:
	/* Build For Cells */
	virtual void BuildTargetCells(
		ABossActor* Boss,
		TArray<FGridPoint>& OutCells, int32 PatternNum);

	virtual void ExecutePattern(
		ABossActor* Boss,
		const TArray<FGridPoint>& InLockedCells,
		const TArray<ACoinActor*>& InLockedTargets,
		const TArray<ABase_OtherActor*>& InLockedOthers,
		int32 PatternNum);

	/* For Real Damage */
	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual void ExecuteDamage(const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers, ABossActor* Boss, int32 Damage);
	
	/* Getter */
	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FLinearColor GetTelegraphColor() const { return TelegraphColor; }

	UFUNCTION(BlueprintCallable, Category = "Pattern")
	virtual FString GetPatternName(int32 PatternIndex) const { return PatternData[PatternIndex].PatternName; }

	/* 이펙트 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Pattern | Effect")
	void PlayPatternEffect(int32 PatternNum, FVector EffectLocation);
	virtual void PlayPatternEffect_Implementation(int32 PatternNum, FVector EffectLocation);
};
