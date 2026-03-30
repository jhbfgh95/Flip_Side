// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinSlotIndexButtonLock.generated.h"

UCLASS()
class FLIP_SIDE_API ACoinSlotIndexButtonLock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoinSlotIndexButtonLock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* LockPanelMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* LockImageMesh;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* LockPanelTimeline;
	

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UCurveFloat* LockPanelCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector PanelMoveDirection;
	
	FVector PanelStartVector;	
	FVector PanelArriveVector;

	bool IsOpen;
private:
	UFUNCTION()
	void PanelMovement(float Value);

	UFUNCTION()
	void UnlockSlotPanel();

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 SlotIndexNum = -1;
};
