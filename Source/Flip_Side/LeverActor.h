#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LeverActor.generated.h"

UCLASS()
class FLIP_SIDE_API ALeverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALeverActor();

protected:
	virtual void BeginPlay() override;

	virtual void ExecuteLeverLogic(); // 자식 고유 로직 구현

	virtual void PlayLeverAnimation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lever | State")
    bool bIsPulled = false;

	UPROPERTY(EditAnywhere, Category = "Lever | Animation")
    FRotator StartRotation = FRotator(-60.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Lever | Animation")
    FRotator TargetRotation = FRotator(60.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Lever | Animation")
    float AnimationSpeed = 0.5f;

public:	
	// 레버 고정부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* BaseMesh;

	// 손잡이
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* LeverMesh;

    void OnLeverInteracted();
};