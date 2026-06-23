#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "BossPatternPreviewActor.generated.h"

class AGridActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossPatternWanted);

UCLASS()
class FLIP_SIDE_API ABossPatternPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ABossPatternPreviewActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	FVector GridOrigin = FVector(200000.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	float SpacingX = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	float SpacingY = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	int32 GridXSize = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	int32 GridYSize = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	TSubclassOf<AGridActor> GridActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	FLinearColor AttackHighlightColor = FLinearColor(1.f, 0.f, 0.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	float HighlightFillIntensity = 0.3f;

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ShowPatternPreview(const FAttackAreaSpec& Spec);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ClearPreview();

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void RebuildGrid();

	UPROPERTY(BlueprintAssignable, Category = "Preview")
	FOnBossPatternWanted OnPatternWanted;

private:
	void SpawnGrid();
	void DestroyGrid();

	AGridActor* GetGridActorAt(int32 X, int32 Y) const;
	bool IsInGrid(int32 X, int32 Y) const;

	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> GridActors;

	TArray<FGridPoint> HighlightedCells;
};
