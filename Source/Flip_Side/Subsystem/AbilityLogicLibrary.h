#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTypes/GridTypes.h"
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
	static bool SpearGuardAfterAttack(UWeapon_Action* WeaponContext);
	// 후보 수집, 호버 투영, 실행 직전 검사에서 같은 전방 이동 조건을 사용합니다.
	static bool TryGetSpearGuardDestination(
		const UWeapon_Action* WeaponContext, const class ACoinActor* TargetCoin, FGridPoint& OutDestination);
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
