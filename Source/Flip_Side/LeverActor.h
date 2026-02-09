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

public:	
	// 레버 고정부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* BaseMesh;

	// 실제로 회전할 손잡이
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* LeverMesh;

    // 레이저에 맞았을 때 외부에서 호출해줄 함수
    void OnLeverInteracted();

private:
	// 레버가 이미 당겨졌는지 확인하는 변수
	bool bIsPulled = false;

	// 레버를 부드럽게 돌리는 함수
	void PlayLeverAnimation();
};