#include "Subsystem/AttackLogicLibrary.h"

#include "Actors/Boss/BossActor.h"
#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
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
	// 보스 사망 콜백이 행동 컨텍스트를 초기화해도 마지막 타격의 아이템 효과를 처리합니다.
	TWeakObjectPtr<ACoinActor> Attacker = WeaponContext->GetCasterCoin();
	Boss->ApplyDamageAndReturnHPDamage(Result.RequestedDamage, Attacker.Get());
	Result.ShieldDamage = FMath::Max(0, PreviousShield - Boss->GetCurrentShield());
	Result.HPDamage = FMath::Max(0, PreviousHP - FMath::Max(0, Boss->GetCurrentHP()));
	if (ACoinActor* Coin = Attacker.Get(); IsValid(Coin) && IsValid(Coin->StatComponent))
	{
		Coin->StatComponent->ApplyOnHitStatusEffects(Result);
	}
	return Result;
}
