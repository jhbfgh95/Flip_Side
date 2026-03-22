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


//해금할 코인 선택 했을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSelectUnlockWeapon, EWeaponClass, WeaponClass,int32, WeaponID, bool, IsItemUnlock);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeWeaponClass, EWeaponClass, WeaponClass);

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


private:
	class UUnlockGISubsystem* UnlockSubsystem;
	
private:
	//현재 선택중인 무기 인덱스
	int32 CurrentUnlockWeaponID = -1;
	EWeaponClass CurrentUnlockWeaponClass;

public:
	FSelectUnlockWeapon OnSelectUnlockWeapon;
	FChangeWeaponClass OnChangeUnlockWaeponClass;

public:
	//무기 선택
	void SelectUnlockWeapon(EWeaponClass WeaponClass, int32 WeaponID, bool IsWeaponUnlock);

	void UnlockCurrentWeapon();

	int32 GetCurrentUnlockWeaponID();

	EWeaponClass GetCurrentWeaponClass();

	void ChangeUnlockWeaponClass(EWeaponClass WeaponClass);

public:
	int32 GetWeaponArrayNum(EWeaponClass WeaponType);
	int32 GetWeaponIDByIndex(EWeaponClass WeaponType,int32 index);
};
