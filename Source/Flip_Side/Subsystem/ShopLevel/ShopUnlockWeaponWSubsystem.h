// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/FlipSide_Enum.h"
#include "WeaponDataTypes.h"
#include "ShopUnlockWeaponWSubsystem.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockWeaponFWarning, int32, WarningCode);


UCLASS()
class FLIP_SIDE_API UShopUnlockWeaponWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& World) override;

private:
	class UDataManagerSubsystem* DM;
	const TArray<FFaceData>* TankWeapons;
	const TArray<FFaceData>* DealWeapons;
	const TArray<FFaceData>* UtilWeapons;

	TArray<FFaceData> UnlockWeapons;
private:
	class UUnlockGISubsystem* UnlockSubsystem;
	class UMoneyGISubsystem* MoneySubsystem;
	
public:
	FUnlockWeaponFWarning OnUnlockWeaponWarning;
public:
	void UnlockWeapon(int32 WeaponID);

public:
	int32 GetWeaponArrayNum(EWeaponClass WeaponType);
	int32 GetWeaponIDByIndex(EWeaponClass WeaponType,int32 index);
	void UnlockWeaponWarning(int32 WarningCode);
	const TArray<FFaceData> GetUnlockWeapons();
};
