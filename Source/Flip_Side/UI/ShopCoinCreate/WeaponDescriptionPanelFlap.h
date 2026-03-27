// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDescriptionPanelFlap.generated.h"

UCLASS()
class FLIP_SIDE_API AWeaponDescriptionPanelFlap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDescriptionPanelFlap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	class UCoinCreateWSubsystem* CoinCreateSubsystem;

private:
	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class USceneComponent* LeftFlapRootScene;
	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class USceneComponent* RightFlapRootScene;

	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LeftFlap;
	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RightFlap;
	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class UTimelineComponent* FlapTimeline;

private:
	
	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class UCurveFloat* FlapCurve;

	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	FRotator TargetRotator;

	FRotator StartRotatorL;
	FRotator ArriveRotatorL;

	
	FRotator StartRotatorR;
	FRotator ArriveRotatorR;



private:

	UFUNCTION(BlueprintCallAble)
	void OpenFlap(int index, bool IsFront);
	UFUNCTION(BlueprintCallAble)
	void CloseFlap();
	UFUNCTION()
	void FlapMove(float Value);


	UFUNCTION()
	void FinishOpenFlap();

};
