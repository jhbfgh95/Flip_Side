#pragma once


#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "BossAttackTypes.h"
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

UCLASS()
class FLIP_SIDE_API UGridManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	FVector GridOrigin = FVector(1440.f, -2460.f, -70.f);

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

	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CollectOccupiedCoins(TArray<FCoinOnGridInfo>& OutCoins) const;

	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	AGridActor* GetGridActor(const FGridPoint& P) const;

	UFUNCTION(BlueprintCallable, Category = "Grid|Boss")
	void BuildBossAttackCells(const FBossAttackSpec& Spec, TArray<FGridPoint>& OutCells) const;

	/*
	UFUNCTION(BlueprintCallable, Category="Grid|Boss")
	void PreviewBossAttack(const FBossAttackSpec& Spec);

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

private:
	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> GridActors;
};
