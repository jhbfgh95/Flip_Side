// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopCoinManagePanelGear.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinManagePanelGear : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinManagePanelGear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* GearMesh;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* GearTimeline;
	
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* GearCurve;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

private:
	//코인 관련
	FRotator StartGearRotator;
	FRotator ArriveGearRotator;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator GearRotateDirection;
	bool IsGearRotateUp;

public:

	UFUNCTION()
	void RotateGear(float Value);
	UFUNCTION()
	void ActiveGear(bool IsPanelMoveToBottom);

};
