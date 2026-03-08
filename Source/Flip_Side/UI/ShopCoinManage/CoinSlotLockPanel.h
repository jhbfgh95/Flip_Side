// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "CoinSlotLockPanel.generated.h"

UCLASS()
class FLIP_SIDE_API ACoinSlotLockPanel : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoinSlotLockPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	class UStaticMeshComponent* LockPanelMesh;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UBoxComponent* PanelInteractBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* PanelMoveTimeline;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UCurveFloat* PanelMoveCurve;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector PanelMoveDirection;

	FVector PanelStartLocation;
	FVector PanelArriveLocation;
	
private:
	UFUNCTION()
	void MoveLockPanel(float Value);

	void UnlockCoinSlot(); 
protected:
	virtual void InteractLeftClick_Implementation() override;
public:
	void InitLockPanel(bool IsLock);

	
};
