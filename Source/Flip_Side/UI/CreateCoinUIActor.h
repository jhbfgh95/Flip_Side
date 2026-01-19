// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "CreateCoinUIActor.generated.h"

UCLASS()
class FLIP_SIDE_API ACreateCoinUIActor : public AActor
{
	GENERATED_BODY()
	
private:
	//서브 시스템
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USphereComponent* sphereCollision;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* coinMesh;

	
public:	
	// Sets default values for this actor's properties
	ACreateCoinUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ChangeCoinSide();


private:
	
	bool IsCoinFront = true;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FCoinTypeStructure CoinInfo;

	UFUNCTION()
	void UpdateCoinWeapon(int32 WeaponID);

	void SetCoinSideMatarial();

private:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* Timeline;

	FRotator StartRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinRotationCurve;

	//UFUNCTION()
	//void FinishRotationPannel();


	UFUNCTION(BlueprintCallAble)
	void ClickCoin();

	UFUNCTION()
	void RotateCoin(float Value);

};
