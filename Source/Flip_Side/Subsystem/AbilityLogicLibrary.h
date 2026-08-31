#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilityLogicLibrary.generated.h"

/** 공격 전·적중 후·기동 타이밍의 무기 능력만 담당합니다. */
UCLASS()
class FLIP_SIDE_API UAbilityLogicLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool BurgerAfterAttack(class UWeapon_Action* WeaponContext);
	static bool BloodCannonAbsorb(UWeapon_Action* WeaponContext);
	static bool InstallAutoTurret(UWeapon_Action* WeaponContext);
	static bool SniperOnHit(UWeapon_Action* WeaponContext);
	static bool RapidFreezerOnHit(UWeapon_Action* WeaponContext);
	static bool SmokeSuitAfterAttack(UWeapon_Action* WeaponContext);
	static bool ArmorSuitAfterAttack(UWeapon_Action* WeaponContext);
	static bool GauntletOnHit(UWeapon_Action* WeaponContext);
	static bool GauntletAfterAttack(UWeapon_Action* WeaponContext);
	static bool GrantStrikeBuff(UWeapon_Action* WeaponContext);
	static bool MedikitAfterAttack(UWeapon_Action* WeaponContext);
	static bool ShieldDeployAfterAttack(UWeapon_Action* WeaponContext);
	static bool AdrenalineOnHit(UWeapon_Action* WeaponContext);
	static bool AmplificationLensOnHit(UWeapon_Action* WeaponContext);
	static bool EmergencyDeviceAfterAttack(UWeapon_Action* WeaponContext);
	static bool CrushingDrillAfterAttack(UWeapon_Action* WeaponContext);
	static bool CortisolOnHit(UWeapon_Action* WeaponContext);
};
