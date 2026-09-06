#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossWallActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class FLIP_SIDE_API ABossWallActor : public AActor
{
	GENERATED_BODY()

public:
	ABossWallActor();
	void SetBossAreaSize(const FVector2D& InSize);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Wall")
	TObjectPtr<UStaticMeshComponent> WallMesh;

	// Extra space on each side, in world centimeters.
	UPROPERTY(EditAnywhere, Category = "Boss Wall", meta = (ClampMin = "0", Units = "cm"))
	float MarginX = 100.f;

	UPROPERTY(EditAnywhere, Category = "Boss Wall", meta = (ClampMin = "0", Units = "cm"))
	float MarginY = 100.f;

	UPROPERTY(EditAnywhere, Category = "Boss Wall", meta = (ClampMin = "1", Units = "cm"))
	float WallHeight = 100.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Boss Wall")
	FVector2D BossAreaSize = FVector2D(1320.f, 3960.f);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void UpdateWallTransform();
};
