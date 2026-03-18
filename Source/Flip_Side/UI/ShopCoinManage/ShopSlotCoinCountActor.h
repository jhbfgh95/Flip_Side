// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ShopSlotCoinCountActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShopSlotCoinCountActor : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopSlotCoinCountActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* CoinTurnTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UWidgetComponent* CoinCountWidget;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	class UDataManagerSubsystem* DataManagerSubsystem;

	class UW_CountWidget* CoinCountWidgetClass;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinTurnCurve;

	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator CoinTurnRotator;

	FRotator StartRotator;
	FRotator ArriveRotator;
	

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess= "true"))
	int32 CoinSlot;

private:
	UFUNCTION()
	void TurnCoinMovement(float Value);

	UFUNCTION()
	void EndTurnCoin();

	
	UFUNCTION()
	void SetCountWidget(int32 SlotNum, int32 CoinCount);

	UFUNCTION()
	void SetCoinWeapon(int32 SlotNum, EWeaponClass CoinCount);

public:
	void InitCoin();
	
	void ChangeCoinSide();

public:
	virtual void InteractLeftClick_Implementation() override;	

};
