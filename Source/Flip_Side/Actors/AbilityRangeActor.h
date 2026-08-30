#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityRangeActor.generated.h"

class UInstancedStaticMeshComponent;
class USceneComponent;

/** 동일한 윤곽선 Plane 인스턴스를 능력 사거리의 각 그리드 위치에 표시합니다. */
UCLASS()
class FLIP_SIDE_API AAbilityRangeActor : public AActor
{
	GENERATED_BODY()

public:
	AAbilityRangeActor();

	void ShowRangeAtWorldLocations(const TArray<FVector>& WorldLocations);

	UFUNCTION(BlueprintCallable, Category = "Coin|Range Preview")
	void HideRange();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	TObjectPtr<USceneComponent> PreviewRoot;

	/** BP에서 한 칸 크기의 윤곽선 Plane Mesh와 Material을 지정합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	TObjectPtr<UInstancedStaticMeshComponent> RangePlaneInstances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	float PlaneHeightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	FRotator PlaneRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	FVector PlaneScale = FVector::OneVector;
};
