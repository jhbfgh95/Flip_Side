// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ClassSelectButton.generated.h"

UCLASS()
class FLIP_SIDE_API AClassSelectButton : public AActor
{
	GENERATED_BODY()
	
private:
	//서브 시스템
	class UCoinCreateWSubsystem* coinCreateWSubSystem;

	

	UPROPERTY(EditAnywhere, Category = "Info",meta = (AllowPrivateAccess = "true"))
	EWeaponClass weaponClass;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USphereComponent* sphereCollision;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* buttonMesh;
public:	
	// Sets default values for this actor's properties
	AClassSelectButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void SelectedClass();
};
