#include "Subsystem/AttackLogicLibrary.h"

#include "Actors/Boss/BossActor.h"
#include "Actors/CoinActor.h"
#include "Objects/Weapon_Action.h"

FWeaponAttackResult UAttackLogicLibrary::BasicAttack(UWeapon_Action* WeaponContext)
{
	return ApplyBossDamage(WeaponContext, WeaponContext ? WeaponContext->GetFinalAttackPoint() : 0);
}

FWeaponAttackResult UAttackLogicLibrary::SteelPipeAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext)
	{
		return FWeaponAttackResult();
	}

	const FWeaponNumericStats& Stats = WeaponContext->GetSnapshot().FinalNumericStats;
	return ApplyBossDamage(WeaponContext, Stats.AttackPoint + Stats.WeaponPoint);
}

FWeaponAttackResult UAttackLogicLibrary::BloodCannonAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext)
	{
		return FWeaponAttackResult();
	}

	const int32 Damage = WeaponContext->GetFinalAttackPoint() +
		WeaponContext->GetExecutionState().AbsorbedAmount;
	return ApplyBossDamage(WeaponContext, Damage);
}

FWeaponAttackResult UAttackLogicLibrary::ApplyBossDamage(UWeapon_Action* WeaponContext, int32 Damage)
{
	FWeaponAttackResult Result;
	if (!WeaponContext || !IsValid(WeaponContext->GetCasterCoin()))
	{
		return Result;
	}

	Result.bAttackAttempted = true;
	Result.RequestedDamage = FMath::Max(0, Damage);
	ABossActor* Boss = WeaponContext->GetAttackBoss();
	if (!IsValid(Boss))
	{
		return Result;
	}

	Result.bEnemyInRange = true;
	Result.Boss = Boss;
	const int32 PreviousShield = FMath::Max(0, Boss->GetCurrentShield());
	const int32 PreviousHP = FMath::Max(0, Boss->GetCurrentHP());
	Boss->ApplyDamageAndReturnHPDamage(Result.RequestedDamage, WeaponContext->GetCasterCoin());
	Result.ShieldDamage = FMath::Max(0, PreviousShield - Boss->GetCurrentShield());
	Result.HPDamage = FMath::Max(0, PreviousHP - Boss->GetCurrentHP());
	return Result;
}
