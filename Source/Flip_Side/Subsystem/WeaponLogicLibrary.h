// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponLogicLibrary.generated.h"

class UWeapon_Action;

UCLASS()
class FLIP_SIDE_API UWeaponLogicLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
/* 딜러 */
public:
	static void Test_Logic(UWeapon_Action* WeaponContext);

	static void SteelPipe_Logic(UWeapon_Action* WeaponContext);

	static void SteamChainSaw_Logic(UWeapon_Action* WeaponContext);
	//나중에
	static void Buger_Logic(UWeapon_Action* WeaponContext);

	static void BloodCanon_Logic(UWeapon_Action* WeaponContext);
	//나중에
	static void AutoTurretSet_Logic(UWeapon_Action* WeaponContext);

	static void SniperRifle_Logic(UWeapon_Action* WeaponContext);

/* 탱커 */	
public:
	static void Freezer_Logic(UWeapon_Action* WeaponContext);
	
	static void SmokeSuit_Logic(UWeapon_Action* WeaponContext);

	static void ArmorSuit_Logic(UWeapon_Action* WeaponContext);

	static void EnemyOfSpear_Logic(UWeapon_Action* WeaponContext);

	static void Gauntlet_Logic(UWeapon_Action* WeaponContext);

/* 유틸 */
public:
	static void Medikit_Logic(UWeapon_Action* WeaponContext);

	static void CrossShiled_Logic(UWeapon_Action* WeaponContext);

	static void Adrenaline_Logic(UWeapon_Action* WeaponContext);

	static void LockOnLenz_Logic(UWeapon_Action* WeaponContext);

	static void Emergencylifer_Logic(UWeapon_Action* WeaponContext);

	static void Drill_Logic(UWeapon_Action* WeaponContext);

	static void Fixkit_Logic(UWeapon_Action* WeaponContext);
};
