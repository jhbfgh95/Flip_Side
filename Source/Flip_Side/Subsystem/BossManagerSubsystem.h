#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "BossDataTypes.h"
#include "BossManagerSubsystem.generated.h"

class ABossActor;
class UBossPatternBase;
class ACoinActor;
class AGridActor;

USTRUCT(BlueprintType)
struct FLockedBossTarget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 CoinID = 0;

	UPROPERTY(BlueprintReadOnly)
	FGridPoint LockedGrid;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACoinActor> CoinActor = nullptr;
};

USTRUCT(BlueprintType)
struct FBossTurnContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 PickedThemeID = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PickedBossID = 0;

	UPROPERTY(BlueprintReadOnly)
	FString PickedBossName;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UBossPatternBase> CurrentPattern = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FGridPoint> LockedCells;

	UPROPERTY(BlueprintReadOnly)
	TArray<FLockedBossTarget> LockedTargets;

	UPROPERTY(BlueprintReadOnly)
	bool bPrepared = false;

	void Reset()
	{
		PickedThemeID = 0;
		PickedBossID = 0;
		PickedBossName.Reset();
		CurrentPattern = nullptr;
		LockedCells.Reset();
		LockedTargets.Reset();
		bPrepared = false;
	}
};

UCLASS()
class FLIP_SIDE_API UBossManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Data")
	TArray<FBossData> AllBossData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spawn")
	FVector BossSpawnLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spawn")
	FRotator BossSpawnRotation = FRotator::ZeroRotator;

	UPROPERTY()
	TObjectPtr<ABossActor> CurrentBoss = nullptr;

	UPROPERTY()
	FBossTurnContext TurnContext;

	FTimerHandle TelegraphTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool SpawnBossForStage(int32 StageIndex);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	ABossActor* GetCurrentBoss() const { return CurrentBoss; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool StartBossTurn();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void ExecuteCurrentPattern();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void ClearCurrentTurn();

	void GetCoinsOnCells(const TArray<FGridPoint>& Cells, TArray<ACoinActor*>& OutCoins) const;
	void ApplyDamageToLockedTargets(const TArray<ACoinActor*>& LockedTargets, int32 Damage);

private:
	bool PickRandomBossDataForStage(int32 StageIndex, FBossData& OutBossData) const;
	bool PickRandomThemeFromStageBosses(const TArray<FBossData>& StageBosses, int32& OutThemeID) const;
	bool PickRandomBossFromTheme(const TArray<FBossData>& StageBosses, int32 ThemeID, FBossData& OutBossData) const;

	bool PrepareCurrentPattern();
	void ShowTelegraphPreview(const TArray<FGridPoint>& Cells, const FLinearColor& Color);
	void ClearTelegraphPreview(const TArray<FGridPoint>& Cells);
	bool IsCellIncluded(const FGridPoint& P, const TArray<FGridPoint>& Cells) const;
	void BuildLockedTargetsFromCells(const TArray<FGridPoint>& Cells, TArray<FLockedBossTarget>& OutLockedTargets) const;
	bool IsStillOnLockedCell(const FLockedBossTarget& LockedTarget) const;
};