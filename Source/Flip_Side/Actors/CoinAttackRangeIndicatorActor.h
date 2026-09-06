#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinAttackRangeIndicatorActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/**
 * 코인 공격 사거리의 직선 몸통과 뭉툭한 끝부분만 표시합니다.
 * 시작 괄호는 코인마다 BP에서 위치를 맞출 수 있도록 ACoinActor가 소유합니다.
 */
UCLASS()
class FLIP_SIDE_API ACoinAttackRangeIndicatorActor : public AActor
{
	GENERATED_BODY()

public:
	ACoinAttackRangeIndicatorActor();

	/**
	 * 공격 시작/끝 셀의 중심을 기준으로 실제 그리드 메시 경계에 Body와 EndCap을 맞춥니다.
	 * 시작 셀과 끝 셀이 같으면 몸통을 숨기고 EndCap만 표시합니다.
	 */
	bool ShowRange(
		const FVector& AttackStartCellWorldLocation,
		const FVector& AttackEndCellWorldLocation,
		const FVector& ForwardWorldDirection
	);

	UFUNCTION(BlueprintCallable, Category = "Coin|Range Preview")
	void HideRange();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	TObjectPtr<USceneComponent> PreviewRoot;

	/** Blender 로컬 +Y 방향이 길이 방향인 직선 메시를 BP에서 지정합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	TObjectPtr<UStaticMeshComponent> LineBodyMesh;

	/** 로컬 +X가 공격 진행 방향을 바라보도록 만든 뭉툭한 끝 메시를 BP에서 지정합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	TObjectPtr<UStaticMeshComponent> EndCapMesh;

	/** Blender에서 제작한 LineBodyMesh의 실제 +Y 길이(cm)입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview", meta = (ClampMin = "1.0"))
	float LineBodyNativeLength = 100.0f;

	/** Body와 EndCap 사이의 틈을 감추기 위해 Body를 EndCap 방향으로 더 늘리는 길이(cm)입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview", meta = (ClampMin = "0.0"))
	float BodyEndOverlap = 20.0f;

	/** 그리드보다 위에 배치하기 위한 월드 Z 오프셋입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	float PreviewHeightOffset = 15.0f;

	/* 카메라 화각 따라서 EndCap을 더 코인쪽으로 배치하기 위한 값*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	float CamOffset = 10.0f;

	/**
	 * 간격(440cm)을 포함한 셀 중심 거리가 아닌, 실제 보이는 그리드 메시의 공격 방향 길이입니다.
	 * BP_Grid 메시 크기가 바뀌면 BP에서 같이 조정합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview", meta = (ClampMin = "1.0"))
	float GridVisualLength = 400.0f;

	/** Body 없이 EndCap만 표시되는 1칸 공격 사거리의 EndCap Y Scale입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview", meta = (ClampMin = "0.0"))
	float SingleCellEndCapYScale = 0.8f;

	/** LineBodyMesh의 로컬 +Y를 공격 방향으로 맞춘 뒤 추가 보정이 필요할 때 BP에서 조정합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin|Range Preview")
	FRotator MeshRotationOffset = FRotator::ZeroRotator;

private:
	/** BP의 EndCapMesh Transform에서 지정한 기본 Scale입니다. */
	FVector DefaultEndCapRelativeScale = FVector::OneVector;
};
