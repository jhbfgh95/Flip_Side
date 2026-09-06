#include "BossGimmick_Swamp.h"
#include "BossActor.h"
#include "CoinActor.h"
#include "Component_Status.h"
#include "DataTypes/BossDataTypes.h"
#include "DataTypes/CoinStatDataTypes.h"

namespace
{
	// 0/1번 패턴이 부여한 늪 디버프를 갖고 있는지 확인 (다른 출처 디버프는 무시)
	bool CoinHasSwampDebuff(ACoinActor* Coin)
	{
		if (!IsValid(Coin) || !IsValid(Coin->StatComponent))
		{
			return false;
		}

		for (const FStatusEffectInstance& Effect : Coin->StatComponent->GetStatusEffects())
		{
			if (Effect.BuffTypeID == BossBuffTypeID::SwampWeaponPowerDown ||
				Effect.BuffTypeID == BossBuffTypeID::SwampAttackPowerDown)
			{
				return true;
			}
		}
		return false;
	}

	void ApplySwampDebuff(ABossActor* Boss, ACoinActor* Coin, int32 BuffTypeID, int32 DurationTurns, int32 WeaponPointDelta, int32 AttackPointDelta)
	{
		if (!IsValid(Coin) || !IsValid(Coin->StatComponent))
		{
			return;
		}

		FStatusEffectInstance Debuff;
		Debuff.BuffTypeID = BuffTypeID;
		Debuff.SourceType = EStatusEffectSourceType::Boss;
		Debuff.SourceDataID = Boss ? Boss->GetBossID() : INDEX_NONE;
		Debuff.Polarity = EStatusPolarity::Debuff;
		Debuff.DurationType = EBuffDurationType::PersistentInBattle;
		Debuff.StackPolicy = EStatusStackPolicy::NonStackable;
		Debuff.RemainingTurns = DurationTurns;
		Debuff.Modifier.WeaponPoint = WeaponPointDelta;
		Debuff.Modifier.AttackPoint = AttackPointDelta;

		Coin->StatComponent->AddStatusEffect(Debuff);
	}
}

void UBossGimmick_Swamp::OnBeforePatternExecute(ABossActor* Boss, FBossPhaseContext& Context)
{
	if (!Boss) return;

	PendingPatternIndex = Context.CurrentPatternIndex;

	switch (Context.CurrentPatternIndex)
	{
	case 0: // 1x6, 무기력 디버프 - 고정 피해 1
	case 1: // 1x6, 공격력 디버프 - 고정 피해 1
		Context.BonusDamage = 1 - Context.BaseDamage;
		break;
	case 2: // 2x2, 디버프 보유 여부로 코인별 개별 데미지 - 엔진 기본(균일) 데미지 스킵
		Context.bSkipAttack = true;
		break;
	default:
		break;
	}
}

void UBossGimmick_Swamp::OnPatternExecute(
	ABossActor* Boss,
	const TArray<FGridPoint>& LockedCells,
	const TArray<ACoinActor*>& LockedTargets,
	const TArray<ABase_OtherActor*>& LockedOthers)
{
	// boss_gimmick(id=5, "늪") 기준: ParamIntA=지속 턴수, ParamIntB=무기력 디버프,
	// ParamFloatA=공격력 디버프, ParamFloatB=기본 피해, ParamFloatC=디버프 보유 시 피해
	switch (PendingPatternIndex)
	{
	case 0:
		for (ACoinActor* Coin : LockedTargets)
		{
			ApplySwampDebuff(Boss, Coin, BossBuffTypeID::SwampWeaponPowerDown, GimmickData.ParamIntA, GimmickData.ParamIntB, 0);
		}
		break;
	case 1:
		for (ACoinActor* Coin : LockedTargets)
		{
			ApplySwampDebuff(Boss, Coin, BossBuffTypeID::SwampAttackPowerDown, GimmickData.ParamIntA, 0, FMath::RoundToInt(GimmickData.ParamFloatA));
		}
		break;
	case 2:
		for (ACoinActor* Coin : LockedTargets)
		{
			if (!IsValid(Coin) || !IsValid(Coin->StatComponent))
			{
				continue;
			}
			const int32 Damage = CoinHasSwampDebuff(Coin)
				? FMath::RoundToInt(GimmickData.ParamFloatC)
				: FMath::RoundToInt(GimmickData.ParamFloatB);
			Coin->StatComponent->ApplyDamage(Damage, Boss);
		}
		break;
	default:
		break;
	}

	PendingPatternIndex = -1;
}
