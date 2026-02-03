// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "CreateCoinUIActor.generated.h"

UCLASS()
class FLIP_SIDE_API ACreateCoinUIActor : public AActor
{
	GENERATED_BODY()
	
private:
	//서브 시스템
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PressMachineMesh;
	
public:	
	// Sets default values for this actor's properties
	ACreateCoinUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ChangeCoinSide();


private:
	
	bool IsCoinFront = true;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FCoinTypeStructure CoinInfo;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTexture2D* FrontIconTexture;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTexture2D* BackIconTexture;
	
	EWeaponClass WeaponType = EWeaponClass::Tank;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TArray<FLinearColor> TypeColors;

	UFUNCTION()
	void UpdateCoinWeapon(int32 WeaponID);

	UFUNCTION()
	void UpdateWeaponClass(EWeaponClass weponClass);
	UFUNCTION()
	void SetCoinSideMatarial();

	const FFaceData* FrontFaceData; 
	
	const FFaceData* BackFaceData;

private:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* Timeline;

	FRotator StartRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinRotationCurve;

	//UFUNCTION()
	//void FinishRotationPannel();


	UFUNCTION(BlueprintCallAble)
	void ClickCoin();

	UFUNCTION()
	void RotateCoin(float Value);

	UFUNCTION()
	void InitCoin(FCoinTypeStructure CoinValue, EWeaponClass weponClass);
private:
	UPROPERTY(EditAnywhere,Category = "PressMachine",meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* PressMachineTimeline;

	UPROPERTY(EditAnywhere, Category = "PressMachine", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PressMachineCurve;

	FVector MachineStartLocation;
	FVector MachineEndLocation;

	UFUNCTION()
	void PressCoin(float Value);

	void ResetSideTexture();

	void SetCoinSide(const FFaceData*& faceData , int32 ID);
};
