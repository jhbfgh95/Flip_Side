// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "ShopWeaponDataWSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UShopWeaponDataWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& World) override;

private:
	class UDataManagerSubsystem* DM;
	const TArray<FFaceData>* TankWeapons;
	const TArray<FFaceData>* DealWeapons;
	const TArray<FFaceData>* UtilWeapons;


public:
	
	const FFaceData* GetWeaponDataByIndex(EWeaponClass weponClass, int32 Index) const;

	const FFaceData* GetWeaponDataByID(EWeaponClass weponClass, int32 ID) const;

	int32 GetWeaponIndexByID(EWeaponClass weponClass, int32 ID);

	int32 GetWeaponArrayNum(EWeaponClass weponClass);
};
