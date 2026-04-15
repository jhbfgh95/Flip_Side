// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "ShopCoinSlotCountButton.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinSlotCountButton : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	class UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* ButtonPressTimeline;

public:	
	// Sets default values for this actor's properties
	AShopCoinSlotCountButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UCurveFloat* PressCurve;

	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector PressDirection;

	FVector StartLocation;
	FVector ArriveLocation;
private:
	UFUNCTION()
	void MoveButton(float Value);
protected:
	virtual void InteractLeftClick_Implementation() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsIncrease = false;

private:
	int32 IndexNum = 0;


};
