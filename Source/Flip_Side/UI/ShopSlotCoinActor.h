// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopSlotCoinActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShopSlotCoinActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopSlotCoinActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector StartLocation;
	FVector EndLocation;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* CoinMoveTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinMoveCurve;

	UFUNCTION()
	void FinishCoinTimeline();

	bool bISCoinInSlot;

public:

	void AddCoin();
	void RemoveCoin();
	//코인 넣음
	UFUNCTION()
	void MoveCoin(float Value);
	//코인 뻄
	//UFUNCTION()
	//void RemoveCoin(float Value);

};

