// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "ShopCoinManageCoin.generated.h"
UCLASS()
class FLIP_SIDE_API AShopCoinManageCoin : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinManageCoin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* CoinTurnTimeline;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	class UDataManagerSubsystem* DataManagerSubsystem;
	class AShopController_FlipSide* ShopController;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinTurnCurve;

	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator CoinTurnRotator;

	FRotator StartRotator;
	FRotator ArriveRotator;
private:
	bool IsFront = true;
	FRotator InitRotator;
private:
	UFUNCTION()
	void TurnCoinMovement(float Value);
	
	
	UFUNCTION()
	void FinishedTurnCoin();

public:
	void InitCoin();
	
	void ChangeCoinSide();


public:
	virtual void InteractLeftClick_Implementation() override;	
};
