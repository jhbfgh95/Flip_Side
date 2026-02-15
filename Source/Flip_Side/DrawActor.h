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

	UPROPERTY(EditAnywhere, Category = "Draw | Move", meta = (AllowPrivateAccess = "true"))
	float TargetInXLoc = 1215.f;

	UPROPERTY(EditAnywhere, Category = "Draw | Move", meta = (AllowPrivateAccess = "true"))
	float TargetOutXLoc = 155.f;

	UPROPERTY(EditAnywhere, Category = "Draw | Move", meta = (AllowPrivateAccess = "true"))
	float MoveTime = 0.6f;

	float MoveElapsedTime = 0.f;

	bool bIsDrawOut = true;

	FTimerHandle MovementTimer;

	FTimerHandle LeverTimer;

public:	
	ADrawActor();

	void DoDrawAct(bool bIsOut);


protected:
	virtual void BeginPlay() override;

	void WaitForLever();

	void DrawAct();

public:	
	virtual void Tick(float DeltaTime) override;

};
