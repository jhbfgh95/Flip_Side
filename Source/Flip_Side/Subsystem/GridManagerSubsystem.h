#pragma once


#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "GridManagerSubsystem.generated.h"

class AGridActor;
class ACoinActor;

USTRUCT(BlueprintType)
struct FCoinOnGridInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 CoinID = 0;
	UPROPERTY(BlueprintReadOnly) FGridPoint GridXY;
	ACoinActor* CoinActor = nullptr;
};

USTRUCT()
struct FCellDoorFxState
{
	GENERATED_BODY()

public:
	FTimerHandle Phase1Tick;
	FTimerHandle Phase2Tick;

	float PhaseDuration = 1.5f;

	float Phase1StartTime = 0.f;
	float Phase2StartTime = 0.f;
};

UCLASS()
class FLIP_SIDE_API UGridManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	FVector GridOrigin = FVector(1340.f, -2560.f, -100.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	float SpacingX = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	float SpacingY = 440.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	int32 GridXSize = 8; // 가로

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	int32 GridYSize = 5; // 세로

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void InitGrid(int32 InGridXSize, int32 InGridYSize);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void ClearGrid();

	UFUNCTION(BlueprintCallable)
	void PlaySingleCellDoorOpenFx(int32 GridX, int32 GridY, float PhaseDuration = 1.5f);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CollectOccupiedCoins(TArray<FCoinOnGridInfo>& OutCoins) const;


	UFUNCTION(BlueprintCallable, Category = "Grid|Boss")
	void BuildBossAttackCells(const FAttackAreaSpec& Spec, TArray<FGridPoint>& OutCells) const;

	/*
	UFUNCTION(BlueprintCallable, Category="Grid|Boss")
	void PreviewBossAttack(const FAttackAreaSpec& Spec);

	UFUNCTION(BlueprintCallable, Category="Grid|Boss")
	void ClearBossAttackPreview();
	*/

	using FCoinPred = TFunctionRef<bool(const FCoinOnGridInfo&)>;

	void BuildCoinTargetCells(
		FCoinPred Predicate,
		TArray<FGridPoint>& OutCells
	) const;

private:
	void InstanceGrid();

	UPROPERTY(Transient)
	TMap<FGridPoint, FCellDoorFxState> DoorFxByCell;

	bool IsInGrid(int32 X, int32 Y) const;

	AGridActor* GetGridActorAt(int32 X, int32 Y) const;

	void StopDoorFx(const FGridPoint& Cell);

	void TickPhase1(FGridPoint Cell);
	void StartPhase2(FGridPoint Cell);
	void TickPhase2(FGridPoint Cell);

private:
	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> GridActors;
};
