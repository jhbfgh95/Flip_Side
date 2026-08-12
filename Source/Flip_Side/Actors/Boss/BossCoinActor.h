/*
	보스 앞 가장 뒤 가운데 3x3 칸에 고정으로 배치되는 발판 액터.
	ABossActor와는 완전히 독립적 - 레벨에 직접 하나 배치해서 씀.
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

public:
	ABossCoinActor();

protected:
	virtual void BeginPlay() override;
};
