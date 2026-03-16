// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "SelectUnlockWeaponUIActor.generated.h"

UCLASS()
class FLIP_SIDE_API ASelectUnlockWeaponUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASelectUnlockWeaponUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	

private:
	class UShopUnlockWeaponWSubsystem* ShopUnlockWeaponSubSystem;
	class UShopWeaponDataWSubsystem* WeaponDataSubsystem;	
	class UUnlockGISubsystem* UnlockSubsystem;
	class AShopController_FlipSide* ShopContoller;
private:
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class USceneComponent* RootScene;
	
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class USceneComponent* LockCoinScene;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LockMesh;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class UTimelineComponent* RiseTimeline;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class UTimelineComponent* JitterTimeline;
	
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class UTimelineComponent* SizeTimeline;

private:
	//const FFaceData* UnlockWeaponData; 
	UFUNCTION()
	void SetUnlockCoin(EWeaponClass WeaponClass, int32 index, bool IsItemUnlock);

private:
	
	UPROPERTY(EditAnywhere, meta=(AccessPrivateAllow))
	class UCurveFloat* RiseCurve;
	
	UPROPERTY(EditAnywhere, meta=(AccessPrivateAllow))
	class UCurveFloat* JitterCurve;

	UPROPERTY(EditAnywhere, meta=(AccessPrivateAllow))
	class UCurveFloat* SizeCurve;
private:
	UPROPERTY(EditAnywhere, meta=(AccessPrivateAllow))
	FVector RiseDirection;

	FVector StartLocation;
	FVector ArriveLocation;

	UPROPERTY(EditAnywhere, meta=(AccessPrivateAllow))
	float JitterValue;

	UPROPERTY(EditAnywhere, meta=(AccessPrivateAllow))
	FVector MaxCoinSize;

	FVector StartSize;


private:

	UFUNCTION()
	void UnlockWeapon(EWeaponClass WeaponClass, int32 AddIndex);

	UFUNCTION()
	void JitterCoin(float Value);
	UFUNCTION()
	void RiseCoin(float Value);

	UFUNCTION()
	void ChangeSizeCoin(float Value);


	UFUNCTION()
	void FinishedRiseCoin();
	UFUNCTION()
	void FinishedJitterCoin();
	UFUNCTION()
	void FinishedChangeSizeCoin();

	void FinishedUnlockCoinAnim();
};
