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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class UShopUnlockWeaponWSubsystem* ShopUnlockWeaponSubSystem;
	class UShopWeaponDataWSubsystem* WeaponDataSubsystem;

private:
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;

private:
	//const FFaceData* UnlockWeaponData; 
	UFUNCTION()
	void SetUnlockCoin(EWeaponClass WeaponClass, int32 index, bool IsItemUnlock);

};
