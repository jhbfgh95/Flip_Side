#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "GridManagerSubsystem.generated.h"

class AGridActor;

USTRUCT(BlueprintType)
struct FCoinOnGridInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 CoinID = 0;
	UPROPERTY(BlueprintReadOnly) FGridPoint GridXY;
};

UCLASS()
class FLIP_SIDE_API UGridManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// 스폰 설정 (너가 준 고정값)
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

private:
	void InstanceGrid();

private:
	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> GridActors;
};
