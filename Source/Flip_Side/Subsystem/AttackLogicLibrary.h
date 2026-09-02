#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTypes/WeaponDataTypes.h"
#include "AttackLogicLibrary.generated.h"

/** 보스 기본 공격만 담당하며 능력 로직이나 대상 선택 상태를 직접 제어하지 않습니다. */
UCLASS()
class FLIP_SIDE_API UAttackLogicLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FWeaponAttackResult BasicAttack(class UWeapon_Action* WeaponContext);
	static FWeaponAttackResult SteelPipeAttack(UWeapon_Action* WeaponContext);
	static FWeaponAttackResult BloodCannonAttack(UWeapon_Action* WeaponContext);

	/** 적중 후 추가 피해 능력도 동일한 보호막·HP 결과 규약을 사용합니다. */
	static FWeaponAttackResult ApplyBossDamage(UWeapon_Action* WeaponContext, int32 Damage);
};
