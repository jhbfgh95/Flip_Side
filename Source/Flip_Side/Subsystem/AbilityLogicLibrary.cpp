#include "Subsystem/AbilityLogicLibrary.h"

#include "Actors/Boss/BossActor.h"
#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
#include "Actors/GridActor.h"
#include "Actors/Others/Base_OtherActor.h"
#include "Actors/Others/Turret_OtherActor.h"
#include "DataTypes/BossDataTypes.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Objects/Weapon_Action.h"
#include "Subsystem/AttackLogicLibrary.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/OthersWSubsystem.h"

namespace
{
	FStatusEffectInstance MakeWeaponStatus(
		const UWeapon_Action* Context,
		int32 BuffTypeID,
		EBuffDurationType DurationType = EBuffDurationType::TurnOnly)
	{
		FStatusEffectInstance Status;
		Status.BuffTypeID = BuffTypeID;
		Status.SourceType = EStatusEffectSourceType::Coin;
		Status.SourceDataID = Context ? Context->GetSnapshot().WeaponID : INDEX_NONE;
		Status.Polarity = EStatusPolarity::Buff;
		Status.DurationType = DurationType;
		Status.StackPolicy = EStatusStackPolicy::Stack;
		return Status;
	}

	UComponent_Status* GetFirstTargetStatus(UWeapon_Action* Context)
	{
		if (!Context)
		{
			return nullptr;
		}

		for (ACoinActor* Coin : Context->GetInRangeCoins())
		{
			if (IsValid(Coin) && IsValid(Coin->StatComponent))
			{
				return Coin->StatComponent;
			}
		}
		return nullptr;
	}

	void AddAdditionalDamageToState(UWeapon_Action* Context, const FWeaponAttackResult& Result)
	{
		if (Context)
		{
			Context->GetExecutionState().TotalDamageDealt += Result.GetTotalDamage();
		}
	}
}

bool UAbilityLogicLibrary::BurgerAfterAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext || !IsValid(WeaponContext->GetCasterCoin()) ||
		!IsValid(WeaponContext->GetCasterCoin()->StatComponent))
	{
		return false;
	}

	UComponent_Status* StatusComponent = WeaponContext->GetCasterCoin()->StatComponent;
	const int32 WeaponID = WeaponContext->GetSnapshot().WeaponID;
	if (WeaponContext->GetExecutionState().TotalDamageDealt <= 0)
	{
		StatusComponent->RemoveStatusEffectsByTypeAndSource(
			WeaponBuffTypeID::BurgerStack, EStatusEffectSourceType::Coin, WeaponID);
		return true;
	}

	if (StatusComponent->GetStatusEffectStackCount(
		WeaponBuffTypeID::BurgerStack, EStatusEffectSourceType::Coin, WeaponID) >= 3)
	{
		return true;
	}

	FStatusEffectInstance BurgerStack = MakeWeaponStatus(
		WeaponContext, WeaponBuffTypeID::BurgerStack, EBuffDurationType::PersistentInBattle);
	BurgerStack.Modifier.AttackPoint = WeaponContext->GetFinalBehaviorPoint();
	return StatusComponent->AddStatusEffect(BurgerStack);
}

bool UAbilityLogicLibrary::BloodCannonAbsorb(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext || !IsValid(WeaponContext->GetCasterCoin()) ||
		!IsValid(WeaponContext->GetCasterCoin()->StatComponent))
	{
		return false;
	}

	int32 AbsorbedAmount = 0;
	const int32 WeaponPoint = WeaponContext->GetFinalBehaviorPoint();
	for (ACoinActor* Coin : WeaponContext->GetInRangeCoins())
	{
		if (!IsValid(Coin) || Coin == WeaponContext->GetCasterCoin() || !IsValid(Coin->StatComponent))
		{
			continue;
		}

		const int32 PreviousTotal = Coin->StatComponent->GetHP() + Coin->StatComponent->GetShield();
		Coin->StatComponent->ApplyDamage(WeaponPoint, WeaponContext->GetCasterCoin());
		const int32 CurrentTotal = Coin->StatComponent->GetHP() + Coin->StatComponent->GetShield();
		AbsorbedAmount += FMath::Max(0, PreviousTotal - CurrentTotal);
	}

	WeaponContext->GetExecutionState().AbsorbedAmount += AbsorbedAmount;
	if (AbsorbedAmount <= 0)
	{
		return true;
	}

	FStatusEffectInstance Absorb = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::Absorb);
	Absorb.Modifier.AttackPoint = AbsorbedAmount;
	Absorb.RuntimeValue = AbsorbedAmount;
	return WeaponContext->GetCasterCoin()->StatComponent->AddStatusEffect(Absorb);
}

bool UAbilityLogicLibrary::InstallAutoTurret(UWeapon_Action* WeaponContext)
{
	AGridActor* TargetGrid = WeaponContext ? WeaponContext->GetTargetGrid() : nullptr;
	ACoinActor* CasterCoin = WeaponContext ? WeaponContext->GetCasterCoin() : nullptr;
	if (!IsValid(TargetGrid) || TargetGrid->GetIsOccupied() || !IsValid(CasterCoin))
	{
		return false;
	}

	UWorld* World = TargetGrid->GetWorld();
	UClass* TurretClass = StaticLoadClass(
		ATurret_OtherActor::StaticClass(), nullptr,
		TEXT("/Game/Others/BP_Turret_OtherActor.BP_Turret_OtherActor_C"));
	if (!IsValid(World) || !IsValid(TurretClass))
	{
		return false;
	}

	const FVector2D GridWorldXY = TargetGrid->GetGridWorldXY();
	ATurret_OtherActor* Turret = World->SpawnActor<ATurret_OtherActor>(
		TurretClass, FVector(GridWorldXY.X, GridWorldXY.Y, -50.f), FRotator::ZeroRotator);
	if (!IsValid(Turret))
	{
		return false;
	}

	TargetGrid->SetOccupied(true, EGridOccupyingType::Turret, Turret);
	Turret->SetOccupiedGrid(TargetGrid);
	Turret->InitializeTurret(TargetGrid->GetGridPoint(), WeaponContext->GetSnapshot().AttackAreaSpec);
	Turret->SetTurretAttackPoint(WeaponContext->GetFinalBehaviorPoint());
	if (UOthersWSubsystem* OthersManager = World->GetSubsystem<UOthersWSubsystem>())
	{
		OthersManager->RegisterOther(Turret);
	}
	return true;
}

bool UAbilityLogicLibrary::SniperOnHit(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext || !IsValid(WeaponContext->GetCasterCoin()))
	{
		return false;
	}

	UGridManagerSubsystem* GridManager = WeaponContext->GetWorld()
		? WeaponContext->GetWorld()->GetSubsystem<UGridManagerSubsystem>()
		: nullptr;
	if (!IsValid(GridManager))
	{
		return false;
	}

	const FGridPoint Origin = WeaponContext->GetCasterCoin()->GetDecidedGrid();
	constexpr FGridPoint Directions[] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	bool bAdjacentObstacle = false;
	for (const FGridPoint& Direction : Directions)
	{
		AGridActor* Grid = GridManager->GetGridActor(
			FGridPoint{ Origin.GridX + Direction.GridX, Origin.GridY + Direction.GridY });
		ABase_OtherActor* Other = IsValid(Grid) ? Cast<ABase_OtherActor>(Grid->GetCurrentOccupied()) : nullptr;
		if (IsValid(Other) && Other->GetOtherType() == EOthersType::Wall)
		{
			bAdjacentObstacle = true;
			break;
		}
	}

	if (!bAdjacentObstacle)
	{
		return true;
	}

	const int32 Distance = FMath::Max(1, WeaponContext->GetSnapshot().AttackAreaSpec.ParamB);
	const FWeaponAttackResult BonusResult = UAttackLogicLibrary::ApplyBossDamage(
		WeaponContext, WeaponContext->GetFinalBehaviorPoint() * Distance);
	AddAdditionalDamageToState(WeaponContext, BonusResult);
	return true;
}

bool UAbilityLogicLibrary::RapidFreezerOnHit(UWeapon_Action* WeaponContext)
{
	ABossActor* Boss = WeaponContext ? WeaponContext->GetAttackBoss() : nullptr;
	if (!IsValid(Boss))
	{
		return false;
	}

	const int32 ChancePercent = FMath::Clamp(WeaponContext->GetFinalBehaviorPoint() * 10, 0, 100);
	if (FMath::RandRange(1, 100) <= ChancePercent)
	{
		FCCStructure Stun;
		Stun.CCType = ECCTypes::Stun;
		Stun.CCDuration = 1;
		Boss->ApplyCC(Stun);
	}
	return true;
}

bool UAbilityLogicLibrary::SmokeSuitAfterAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext)
	{
		return false;
	}

	const bool bHit = WeaponContext->GetExecutionState().TotalDamageDealt > 0;
	const int32 ChancePercent = FMath::Clamp(
		(WeaponContext->GetFinalBehaviorPoint() + (bHit ? WeaponContext->GetFinalAttackPoint() : 0)) * 10,
		0, 100);
	bool bAppliedAny = false;
	for (ACoinActor* Coin : WeaponContext->GetInRangeCoins())
	{
		if (!IsValid(Coin) || !IsValid(Coin->StatComponent))
		{
			continue;
		}

		FStatusEffectInstance Dodge = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::SmokeDodge);
		Dodge.ReactiveBehavior = EStatusReactiveBehavior::DodgeChance;
		Dodge.ReactiveMagnitude = ChancePercent;
		bAppliedAny |= Coin->StatComponent->AddStatusEffect(Dodge);
	}
	return bAppliedAny;
}

bool UAbilityLogicLibrary::ArmorSuitAfterAttack(UWeapon_Action* WeaponContext)
{
	UComponent_Status* TargetStatus = GetFirstTargetStatus(WeaponContext);
	ACoinActor* CasterCoin = WeaponContext ? WeaponContext->GetCasterCoin() : nullptr;
	if (!IsValid(TargetStatus) || !IsValid(CasterCoin) || !IsValid(CasterCoin->StatComponent))
	{
		return false;
	}

	auto AddGuard = [WeaponContext](UComponent_Status* StatusComponent)
	{
		FStatusEffectInstance Guard = MakeWeaponStatus(
			WeaponContext, WeaponBuffTypeID::ArmorGuard);
		Guard.ReactiveBehavior = EStatusReactiveBehavior::ReduceNextDamageAndGrantAttack;
		Guard.ReactiveMagnitude = WeaponContext->GetFinalBehaviorPoint();
		Guard.RemainingTriggers = 1;
		return IsValid(StatusComponent) && StatusComponent->AddStatusEffect(Guard);
	};

	// 방탄복은 수동 선택 대상과 시전자 자신에게 각각 다음 피해 1회 방어를 부여합니다.
	const bool bCasterApplied = AddGuard(CasterCoin->StatComponent);
	const bool bTargetApplied = TargetStatus == CasterCoin->StatComponent
		? bCasterApplied
		: AddGuard(TargetStatus);
	return bCasterApplied || bTargetApplied;
}

bool UAbilityLogicLibrary::GauntletOnHit(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext)
	{
		return false;
	}

	const int32 AttackPoint = WeaponContext->GetFinalAttackPoint();
	const int32 BonusChance = FMath::Clamp(23 - AttackPoint, 0, 100);
	if (FMath::RandRange(1, 100) <= BonusChance)
	{
		const int32 BonusDamage = FMath::RandRange(1, FMath::Max(1, AttackPoint * 5));
		const FWeaponAttackResult BonusResult = UAttackLogicLibrary::ApplyBossDamage(WeaponContext, BonusDamage);
		AddAdditionalDamageToState(WeaponContext, BonusResult);
	}
	return true;
}

bool UAbilityLogicLibrary::GauntletAfterAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext || !IsValid(WeaponContext->GetCasterCoin()) ||
		!IsValid(WeaponContext->GetCasterCoin()->StatComponent))
	{
		return false;
	}

	const int32 WeaponPoint = WeaponContext->GetFinalBehaviorPoint();
	const float BossKillChance = FMath::Clamp(WeaponPoint * 1.2f, 0.0f, 100.0f);
	if (FMath::FRandRange(0.0f, 100.0f) < BossKillChance)
	{
		const FWeaponAttackResult KillResult = UAttackLogicLibrary::ApplyBossDamage(
			WeaponContext, TNumericLimits<int32>::Max());
		AddAdditionalDamageToState(WeaponContext, KillResult);
		return true;
	}

	const float SelfDeathChance = WeaponPoint * 20.0f;
	const bool bSelfDeath = SelfDeathChance > 100.0f ||
		FMath::FRandRange(0.0f, 100.0f) < SelfDeathChance;
	if (!bSelfDeath)
	{
		return true;
	}

	if (SelfDeathChance > 100.0f)
	{
		for (ACoinActor* Coin : WeaponContext->GetInRangeCoins())
		{
			if (IsValid(Coin) && IsValid(Coin->StatComponent))
			{
				Coin->StatComponent->ApplyDamage(
					WeaponContext->GetFinalAttackPoint(), WeaponContext->GetCasterCoin());
			}
		}
	}
	WeaponContext->GetCasterCoin()->StatComponent->ApplyDamage(
		TNumericLimits<int32>::Max(), WeaponContext->GetCasterCoin());
	return true;
}

bool UAbilityLogicLibrary::GrantStrikeBuff(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext || !IsValid(WeaponContext->GetCasterCoin()) ||
		!IsValid(WeaponContext->GetCasterCoin()->StatComponent))
	{
		return false;
	}

	const int32 StrikeAmount = FMath::Max(0, WeaponContext->GetExecutionState().TotalDamageDealt);
	WeaponContext->GetExecutionState().StrikeAmount = StrikeAmount;
	if (StrikeAmount <= 0)
	{
		return true;
	}

	FStatusEffectInstance Strike = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::Strike);
	Strike.RuntimeValue = StrikeAmount;
	return WeaponContext->GetCasterCoin()->StatComponent->AddStatusEffect(Strike);
}

bool UAbilityLogicLibrary::MedikitAfterAttack(UWeapon_Action* WeaponContext)
{
	UComponent_Status* TargetStatus = GetFirstTargetStatus(WeaponContext);
	if (!IsValid(TargetStatus))
	{
		return false;
	}

	TargetStatus->ApplyHeal(
		WeaponContext->GetExecutionState().StrikeAmount + WeaponContext->GetFinalBehaviorPoint(),
		WeaponContext->GetCasterCoin());
	return true;
}

bool UAbilityLogicLibrary::ShieldDeployAfterAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext)
	{
		return false;
	}

	bool bAppliedAny = false;
	for (ACoinActor* Coin : WeaponContext->GetInRangeCoins())
	{
		if (!IsValid(Coin) || !IsValid(Coin->StatComponent))
		{
			continue;
		}

		FStatusEffectInstance Shield = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::TemporaryShield);
		Shield.ReactiveBehavior = EStatusReactiveBehavior::TemporaryShield;
		Shield.ReactiveMagnitude = WeaponContext->GetFinalBehaviorPoint();
		bAppliedAny |= Coin->StatComponent->AddStatusEffect(Shield);
	}
	return bAppliedAny;
}

bool UAbilityLogicLibrary::AdrenalineOnHit(UWeapon_Action* WeaponContext)
{
	UComponent_Status* TargetStatus = GetFirstTargetStatus(WeaponContext);
	if (!IsValid(TargetStatus))
	{
		return false;
	}

	FStatusEffectInstance AttackBuff = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::AdrenalineAttack);
	AttackBuff.Modifier.AttackPoint = WeaponContext->GetFinalBehaviorPoint() +
		WeaponContext->GetExecutionState().StrikeAmount;
	return TargetStatus->AddStatusEffect(AttackBuff);
}

bool UAbilityLogicLibrary::AmplificationLensOnHit(UWeapon_Action* WeaponContext)
{
	UComponent_Status* TargetStatus = GetFirstTargetStatus(WeaponContext);
	if (!IsValid(TargetStatus))
	{
		return false;
	}

	FStatusEffectInstance RangeBuff = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::AmplificationRange);
	RangeBuff.Modifier.AttackAreaSpec.ParamB = WeaponContext->GetFinalBehaviorPoint();
	return TargetStatus->AddStatusEffect(RangeBuff);
}

bool UAbilityLogicLibrary::EmergencyDeviceAfterAttack(UWeapon_Action* WeaponContext)
{
	if (!WeaponContext)
	{
		return false;
	}

	bool bAppliedAny = false;
	for (ACoinActor* Coin : WeaponContext->GetInRangeCoins())
	{
		if (!IsValid(Coin) || !IsValid(Coin->StatComponent))
		{
			continue;
		}

		FStatusEffectInstance Survival = MakeWeaponStatus(
			WeaponContext, WeaponBuffTypeID::EmergencySurvival);
		Survival.ReactiveBehavior = EStatusReactiveBehavior::SurviveLethalOnce;
		Survival.RemainingTriggers = 1;
		bAppliedAny |= Coin->StatComponent->AddStatusEffect(Survival);
	}
	return bAppliedAny;
}

bool UAbilityLogicLibrary::CrushingDrillAfterAttack(UWeapon_Action* WeaponContext)
{
	ABase_OtherActor* TargetOther = WeaponContext ? WeaponContext->GetTargetOther() : nullptr;
	ACoinActor* CasterCoin = WeaponContext ? WeaponContext->GetCasterCoin() : nullptr;
	if (!IsValid(TargetOther) || TargetOther->GetOtherType() != EOthersType::Wall ||
		!IsValid(CasterCoin) || !IsValid(CasterCoin->StatComponent))
	{
		return false;
	}

	const int32 PreviousHP = TargetOther->GetHP();
	TargetOther->ApplyDamage(WeaponContext->GetFinalBehaviorPoint(), CasterCoin);
	const int32 AbsorbedAmount = FMath::Max(0, PreviousHP - TargetOther->GetHP());
	if (AbsorbedAmount <= 0)
	{
		return true;
	}

	FStatusEffectInstance Absorb = MakeWeaponStatus(WeaponContext, WeaponBuffTypeID::Absorb);
	Absorb.Modifier.AttackPoint = AbsorbedAmount;
	Absorb.RuntimeValue = AbsorbedAmount;
	return CasterCoin->StatComponent->AddStatusEffect(Absorb);
}

bool UAbilityLogicLibrary::CortisolOnHit(UWeapon_Action* WeaponContext)
{
	UComponent_Status* TargetStatus = GetFirstTargetStatus(WeaponContext);
	if (!IsValid(TargetStatus))
	{
		return false;
	}

	FStatusEffectInstance WeaponPointBuff = MakeWeaponStatus(
		WeaponContext, WeaponBuffTypeID::CortisolWeaponPoint);
	WeaponPointBuff.Modifier.WeaponPoint = WeaponContext->GetFinalBehaviorPoint() +
		WeaponContext->GetExecutionState().StrikeAmount;
	return TargetStatus->AddStatusEffect(WeaponPointBuff);
}
