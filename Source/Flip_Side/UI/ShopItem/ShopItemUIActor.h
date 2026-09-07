// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/ItemDataTypes.h"
#include "ShopItemUIActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShopItemUIActor : public AActor
{
	GENERATED_BODY()
	
	AShopItemUIActor();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UseableItemRootComp;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* UseableItemMesh;
	
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ItemBuyTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemBuyCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator ItemShakeRotator;

	FRotator StartRotator;
	FRotator ArriveRotator;

	
    UMaterialInstanceDynamic* MID;

private:

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
    FItemData ShopItemData;
	
public:
	void SetItemData(const FItemData& SelectItemData);
	void PlayBuyAnimation();
	void RemoveMaterial();

private:
	void SetItemMaterial();

	UFUNCTION()
	void ItemBuyMovement(float Value);
};
