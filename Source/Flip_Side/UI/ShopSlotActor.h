// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotActor.h"
#include "ShopSlotActor.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopSlotActor : public AActor
{
	GENERATED_BODY()
	
	AShopSlotActor();
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollition;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* SlotMesh;

protected:
	virtual void BeginPlay() override;

private:
	class UShopCoinWSubsystem* CoinSubSystem;

private:
	int32 SlotCoinCount =0;

public:
	TArray<class AShopSlotCoinActor*> SlotCoins;

	void AddCoin();

	void RemoveCoin();
};
