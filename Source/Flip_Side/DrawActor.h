#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawActor.generated.h"

UCLASS()
class FLIP_SIDE_API ADrawActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Draw | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DrawRootComp;

	UPROPERTY(EditAnywhere, Category = "Draw | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DrawMesh;


public:	
	ADrawActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
