// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "PlayerItemInven.generated.h"

UCLASS()
class FLIP_SIDE_API APlayerItemInven : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerItemInven();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class USceneComponent* RootScene;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UStaticMeshComponent* InvenMesh;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UBoxComponent* HandleCollision;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UTimelineComponent* InvenMoveTimeline;


private:
	TArray<USceneComponent*> ChildComponents;
	TArray<class AShopPlayerItemActor*> PlayerItems;

private:
	class UShopItemWSubsystem* ShopItemSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UCurveFloat* InvenMeshCurve;
	FVector StartVector;
	FVector ArriveVector;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	FVector InvenMoveDirection;

bool IsInvenOpen = false;

protected:
	UFUNCTION()
	void InvenMeshMovement(float Value);	

	void ActiveInven();

	UFUNCTION()
	void SetItem(int32 ItemIndex);

public:
	virtual void InteractLeftClick_Implementation() override;
};
