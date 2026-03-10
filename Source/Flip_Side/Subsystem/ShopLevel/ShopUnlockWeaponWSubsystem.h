// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ShopUnlockWeaponWSubsystem.generated.h"

/**
 * 
 */


//해금할 코인 선택 했을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSelectUnlockWeapon, EWeaponClass, WeaponClass,int32, ItemIndex, bool, IsItemUnlock);


UCLASS()
class FLIP_SIDE_API UShopUnlockWeaponWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual void OnWorldBeginPlay(UWorld& World) override;


private:
	class UUnlockGISubsystem* UnlockSubsystem;

private:
	//현재 선택중인 무기 인덱스
	int32 CurrentUnlockWeaponIndex = -1;
	EWeaponClass CurrentUnlockWeaponClass;
public:
	FSelectUnlockWeapon OnSelectUnlockWeapon;

	//무기 해금
	void UnlockWeaponByIndex(int32 WeaponIndex);
	//무기 선택
	void SelectUnlockWeaponByIndex(EWeaponClass WeaponClass, int32 WeaponIndex, bool IsWeaponUnlock);

	void UnlockCurrentWeapon();

	int32 GetCurrentUnlockWeaponIndex();

	EWeaponClass GetCurrentWeaponClass();
};
