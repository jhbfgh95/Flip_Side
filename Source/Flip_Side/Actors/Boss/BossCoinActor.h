/*
	보스 앞 가장 뒤 가운데 3x3 칸에 고정으로 배치되는 발판 액터.
	ABossActor와는 독립적으로 GridManager에서 스폰하며, 준비된 보스 데이터의 아이콘을 사용함.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossCoinActor.generated.h"

UCLASS()
class FLIP_SIDE_API ABossCoinActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "BossCoin | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* BossCoinRoot;

	UPROPERTY(EditAnywhere, Category = "BossCoin | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BossCoinMesh;

	UPROPERTY(EditAnywhere, Category = "BossCoin | Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> TargetArrow;

public:
	ABossCoinActor();

	void SetTargetArrowVisible(bool bVisible);

protected:
	virtual void BeginPlay() override;
};
