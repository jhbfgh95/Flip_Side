// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/WeaponDataTypes.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "InputCoreTypes.h"
#include "ShopCoinUIActor.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UTimelineComponent;
class UCurveFloat;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopCoinSideChanged);

UCLASS()
class FLIP_SIDE_API AShopCoinUIActor : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()

public:
	AShopCoinUIActor();

	UFUNCTION(BlueprintCallable, Category = "Shop Coin")
	void SetFrontCoin(const FFaceData& InFrontFaceData);

	UFUNCTION(BlueprintCallable, Category = "Shop Coin")
	void SetBackCoin(const FFaceData& InBackFaceData);

	// 앞/뒤 면에 적용된 아이콘과 저장된 면 데이터를 모두 초기화합니다.
	UFUNCTION(BlueprintCallable, Category = "Shop Coin")
	void ResetWeaponIcons();

	void ResetFrontWeaponIcon();

	void ResetBackWeaponIcon();

	UFUNCTION(BlueprintCallable, Category = "Shop Coin")
	bool SwapMeshPositions();

	FOnShopCoinSideChanged OnShopCoinSideChanged;

	virtual void InteractLeftClick_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Shop Coin|Component")
	TObjectPtr<USceneComponent> CoinRoot;

	UPROPERTY(EditAnywhere, Category = "Shop Coin|Component")
	TObjectPtr<UStaticMeshComponent> FrontMesh;

	UPROPERTY(EditAnywhere, Category = "Shop Coin|Component")
	TObjectPtr<UStaticMeshComponent> BackMesh;

	UPROPERTY(VisibleAnywhere, Category = "Shop Coin|Component")
	TObjectPtr<UBoxComponent> SwapCollision;

	UPROPERTY(EditAnywhere, Category = "Shop Coin|Material")
	FLinearColor TypeColor;

	UPROPERTY(VisibleAnywhere, Category = "Shop Coin|Animation")
	TObjectPtr<UTimelineComponent> SwapTimeline;

	UPROPERTY(EditAnywhere, Category = "Shop Coin|Animation")
	TObjectPtr<UCurveFloat> SwapCurve;

	UPROPERTY(EditAnywhere, Category = "Shop Coin|Animation", meta = (ClampMin = "0.0"))
	float SwapArcHeight = 75.0f;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> FrontMaterialInstance;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> BackMaterialInstance;

	UPROPERTY(VisibleInstanceOnly, Category = "Shop Coin|Data")
	FFaceData FrontFaceData;

	UPROPERTY(VisibleInstanceOnly, Category = "Shop Coin|Data")
	FFaceData BackFaceData;

	FVector FrontStartLocation;
	FVector BackStartLocation;

	UFUNCTION()
	void UpdateSwapMovement(float Alpha);

	UFUNCTION()
	void FinishSwapMovement();
};
