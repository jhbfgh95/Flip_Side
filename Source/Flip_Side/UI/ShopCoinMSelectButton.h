// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopCoinMSelectButton.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinMSelectButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinMSelectButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 루트 씬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ButtonTimeLine;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* CoinCountWidget;

private:
	class UCurveFloat* ButtonCurve;

	class UShopCoinWSubsystem* ShopCoinSubsystem;
public:
	void SelectCoin();

	void InitButton();


private:
	int32 SlotIndexNum;

};
