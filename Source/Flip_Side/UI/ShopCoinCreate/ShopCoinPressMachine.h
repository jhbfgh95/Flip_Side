// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopCoinPressMachine.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinPressMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinPressMachine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//코인 제작 서브 시스템
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;

private: // 컴포넌트	
//원 콜리전

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PressMachineMesh;

	//프레스 머신 작동 변수
	UPROPERTY(EditAnywhere,Category = "PressMachine",meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* PressMachineTimeline;
	
	//프레스머신 작동 커브
	UPROPERTY(EditAnywhere, Category = "PressMachine", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PressMachineCurve;

	//기계 시작 위치
	FVector MachineStartLocation;
	//기계 종료 위치
	FVector MachineEndLocation;

	//프레스 머신 애니메이션
	UFUNCTION()
	void PressCoin(float Value);

};
