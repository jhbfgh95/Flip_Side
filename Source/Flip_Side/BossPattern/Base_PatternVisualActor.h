#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base_PatternVisualActor.generated.h"

class UCurveFloat;
class UTimelineComponent;

UCLASS()
class FLIP_SIDE_API ABase_PatternVisualActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABase_PatternVisualActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
    class USceneComponent* ActorRoot;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
    class UStaticMeshComponent* ActorMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
    UTimelineComponent* MovementTimeline;

    UPROPERTY(EditAnywhere, Category = "Timeline")
    UCurveFloat* MovementCurve;

    // 시작 위치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    FVector StartLocation;

    // 목표 위치 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    FVector EndLocation;

    // 애니메이션 대기 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
    float TimeBeforeAct = 1.0f;

	// 목표 위치 도달후 파괴 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern | Visual")
    float DestroyDelayTime = 0.3f;

    //매니저에서 호출할 함수
    void PlayBossPatternAct();

private:
    FTimerHandle WaitTimerHandle;

    UFUNCTION()
    void StartMovement();

    UFUNCTION()
    void TimelineProgress(float Value);

    UFUNCTION()
    void TimelineFinished();
};