// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "DataTypes/CoinDataTypes.h"
#include "ShopCoinSlotIndexButton.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinSlotIndexButton : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinSlotIndexButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* ButtonMesh;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* ButtonTimeline;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UWidgetComponent* SlotIndexWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UChildActorComponent* CoinCountActor;

private:

	class AShopSlotCoinCountActor* CountCoin;
	class UW_ShopCoinSlotIndex* SlotIndexWidgetClass;
	class UShopCoinWSubsystem* ShopCoinSubsystem;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UCurveFloat* ButtonCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector ButtonMoveDirection;
	
	FVector ButtonStartVector;	
	FVector ButtonArriveVector;
private:
	UFUNCTION()
	void ButtonMovement(float Value);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 SlotIndexNum = 1;

private:

public:
	virtual void InteractLeftClick_Implementation() override;	
};
