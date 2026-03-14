// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataTypes/FlipSide_Enum.h"
#include "UnlockGISubsystem.generated.h"

/**
 * 
 */


//선택된 코인이 변경되었을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUnlockWeapon, EWeaponClass, WeaponClass, int32, AddIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockCard, int32, CardID);

UCLASS()
class FLIP_SIDE_API UUnlockGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
private:
	TArray<int32> TankUnlockArray;
	TArray<int32> DealUnlockArray;
	TArray<int32> UtilUnlockArray;
	TArray<int32> CardUnlockArray;

public:
	FUnlockWeapon OnWeaponUnlock; 
	FUnlockCard OnUnlockCard;
public:
	int32 GetUnlockCardID(int32 index);
	
	int32 GetUnlockWeaponID(EWeaponClass WeaponClass, int32 index);
	
	int32 GetUnlockWeaponIndex(EWeaponClass WeaponClass, int32 index);

	void UnlockWeapon(EWeaponClass WeaponClass, int32 ID);

	void UnlockCard(int32 ID);

	int32 GetUnlockWeaponArrrayNum(EWeaponClass WeaponClass);

	int32 GetUnlockCardArrayNum();

	int32 GetWeaponUnlockIndexByIndex(EWeaponClass WeaponClass, int32 index);
	
	bool IsWeaponUnlockByID(EWeaponClass WeaponClass, int32 index);

	bool IsCardUnlockByID(int32 ID);
};
