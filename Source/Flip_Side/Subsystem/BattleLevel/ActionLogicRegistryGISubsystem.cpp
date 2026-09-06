#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"

#include "Objects/Item_Action.h"
#include "Subsystem/AbilityLogicLibrary.h"
#include "Subsystem/AttackLogicLibrary.h"
#include "Subsystem/ItemLogicLibrary.h"

namespace
{
	FAbilityTargetRule MakeTargetRule(
		EAbilityTargetFlags Flags,
		EAbilitySelectionMode SelectionMode,
		ERepeatCountSource RepeatSource = ERepeatCountSource::One,
		bool bExcludeCaster = false,
		bool bCoinFieldOnly = false)
	{
		FAbilityTargetRule Rule;
		Rule.TargetFlags = static_cast<int32>(Flags);
		Rule.SelectionMode = SelectionMode;
		Rule.RepeatCountSource = RepeatSource;
		Rule.bExcludeCaster = bExcludeCaster;
		Rule.bCoinFieldOnly = bCoinFieldOnly;
		return Rule;
	}

	FAbilityTargetRule NoTargetRule()
	{
		return MakeTargetRule(EAbilityTargetFlags::None, EAbilitySelectionMode::None);
	}
}

void UActionLogicRegistryGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RegisterAllWeaponLogic();
	RegisterAllItemLogic();
}

void UActionLogicRegistryGISubsystem::RegisterAttackLogic(
	int32 ID,
	FAttackLogic Logic,
	ERepeatCountSource RepeatSource)
{
	FWeaponLogicSet& LogicSet = WeaponLogicMap.FindOrAdd(ID);
	LogicSet.AttackLogic = MoveTemp(Logic);
	LogicSet.AttackRepeatCountSource = RepeatSource;
	if (RepeatSource != ERepeatCountSource::One)
	{
		LogicSet.RepeatChannel = ERepeatLogicChannel::Attack;
	}
}

void UActionLogicRegistryGISubsystem::RegisterAbilityLogic(
	int32 ID,
	EAbilityTiming Timing,
	const FAbilityTargetRule& TargetRule,
	FAbilityLogic Logic,
	FName DebugName)
{
	FWeaponLogicSet& LogicSet = WeaponLogicMap.FindOrAdd(ID);
	FRegisteredAbilityLogic& Ability = LogicSet.AbilityLogics.AddDefaulted_GetRef();
	Ability.Timing = Timing;
	Ability.TargetRule = TargetRule;
	Ability.Logic = MoveTemp(Logic);
	Ability.DebugName = DebugName;
	if (TargetRule.RepeatCountSource != ERepeatCountSource::One)
	{
		LogicSet.RepeatChannel = ERepeatLogicChannel::Ability;
	}
}

void UActionLogicRegistryGISubsystem::RegisterAllWeaponLogic()
{
	WeaponLogicMap.Reset();

	RegisterAttackLogic(1, UAttackLogicLibrary::SteelPipeAttack);
	RegisterAttackLogic(2, UAttackLogicLibrary::BasicAttack, ERepeatCountSource::WeaponPoint);
	RegisterAttackLogic(3, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(4, UAttackLogicLibrary::BloodCannonAttack);
	RegisterAttackLogic(5, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(6, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(7, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(8, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(9, UAttackLogicLibrary::BasicAttack);

	// WeaponID 10 창의 적은 첫 대상 호버 투영 사거리까지 확정한 뒤 등록합니다.
	// RegisterAttackLogic(10, UAttackLogicLibrary::BasicAttack);

	RegisterAttackLogic(11, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(12, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(13, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(14, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(15, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(16, UAttackLogicLibrary::BasicAttack);
	RegisterAttackLogic(17, UAttackLogicLibrary::BasicAttack);
	// WeaponID 18 수리 키트와 19 증기 피스톤은 폐기된 레거시 기획이므로 등록하지 않습니다.
	RegisterAttackLogic(20, UAttackLogicLibrary::BasicAttack);

	RegisterAbilityLogic(3, EAbilityTiming::AfterAttackAlways, NoTargetRule(),
		UAbilityLogicLibrary::BurgerAfterAttack, TEXT("BurgerAfterAttack"));

	RegisterAbilityLogic(4, EAbilityTiming::BeforeAttack,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::AllInRange,
			ERepeatCountSource::One, true),
		UAbilityLogicLibrary::BloodCannonAbsorb, TEXT("BloodCannonAbsorb"));

	RegisterAbilityLogic(5, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::EmptyGrid, EAbilitySelectionMode::Single),
		UAbilityLogicLibrary::InstallAutoTurret, TEXT("InstallAutoTurret"));

	RegisterAbilityLogic(6, EAbilityTiming::OnHit, NoTargetRule(),
		UAbilityLogicLibrary::SniperOnHit, TEXT("SniperOnHit"));

	RegisterAbilityLogic(7, EAbilityTiming::OnHit, NoTargetRule(),
		UAbilityLogicLibrary::RapidFreezerOnHit, TEXT("RapidFreezerOnHit"));

	RegisterAbilityLogic(8, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::AllInRange),
		UAbilityLogicLibrary::SmokeSuitAfterAttack, TEXT("SmokeSuitAfterAttack"));

	RegisterAbilityLogic(9, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::Single,
			ERepeatCountSource::One, true),
		UAbilityLogicLibrary::ArmorSuitAfterAttack, TEXT("ArmorSuitAfterAttack"));

	RegisterAbilityLogic(11, EAbilityTiming::OnHit, NoTargetRule(),
		UAbilityLogicLibrary::GauntletOnHit, TEXT("GauntletOnHit"));
	RegisterAbilityLogic(11, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::AllInRange,
			ERepeatCountSource::One, true, true),
		UAbilityLogicLibrary::GauntletAfterAttack, TEXT("GauntletAfterAttack"));

	RegisterAbilityLogic(12, EAbilityTiming::OnHit, NoTargetRule(),
		UAbilityLogicLibrary::GrantStrikeBuff, TEXT("MedikitStrike"));
	RegisterAbilityLogic(12, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::UpToRepeatCount,
			ERepeatCountSource::WeaponCount, true),
		UAbilityLogicLibrary::MedikitAfterAttack, TEXT("MedikitAfterAttack"));

	RegisterAbilityLogic(13, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::AllInRange),
		UAbilityLogicLibrary::ShieldDeployAfterAttack, TEXT("ShieldDeployAfterAttack"));

	RegisterAbilityLogic(14, EAbilityTiming::OnHit, NoTargetRule(),
		UAbilityLogicLibrary::GrantStrikeBuff, TEXT("AdrenalineStrike"));
	RegisterAbilityLogic(14, EAbilityTiming::OnHit,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::UpToRepeatCount,
			ERepeatCountSource::WeaponCount, true),
		UAbilityLogicLibrary::AdrenalineOnHit, TEXT("AdrenalineOnHit"));

	RegisterAbilityLogic(15, EAbilityTiming::OnHit,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::UpToRepeatCount,
			ERepeatCountSource::WeaponCount, true),
		UAbilityLogicLibrary::AmplificationLensOnHit, TEXT("AmplificationLensOnHit"));

	RegisterAbilityLogic(16, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::AllInRange),
		UAbilityLogicLibrary::EmergencyDeviceAfterAttack, TEXT("EmergencyDeviceAfterAttack"));

	RegisterAbilityLogic(17, EAbilityTiming::AfterAttackAlways,
		MakeTargetRule(EAbilityTargetFlags::Obstacle, EAbilitySelectionMode::UpToRepeatCount,
			ERepeatCountSource::WeaponCount),
		UAbilityLogicLibrary::CrushingDrillAfterAttack, TEXT("CrushingDrillAfterAttack"));

	RegisterAbilityLogic(20, EAbilityTiming::OnHit, NoTargetRule(),
		UAbilityLogicLibrary::GrantStrikeBuff, TEXT("CortisolStrike"));
	RegisterAbilityLogic(20, EAbilityTiming::OnHit,
		MakeTargetRule(EAbilityTargetFlags::Coin, EAbilitySelectionMode::UpToRepeatCount,
			ERepeatCountSource::WeaponCount, true),
		UAbilityLogicLibrary::CortisolOnHit, TEXT("CortisolOnHit"));
}

void UActionLogicRegistryGISubsystem::RegisterAllItemLogic()
{
	ItemMap.Reset();
	ItemMap.Add(1, [](UActionBase* Context)
	{
		UItemLogicLibrary::MeltShieldPotion_Logic(Cast<UItem_Action>(Context));
	});
	ItemMap.Add(2, [](UActionBase* Context)
	{
		UItemLogicLibrary::BloodPotion_Logic(Cast<UItem_Action>(Context));
	});
	ItemMap.Add(3, [](UActionBase* Context)
	{
		UItemLogicLibrary::CleanserPotion_Logic(Cast<UItem_Action>(Context));
	});
	ItemMap.Add(4, [](UActionBase* Context)
	{
		UItemLogicLibrary::PhaseChangePotion_Logic(Cast<UItem_Action>(Context));
	});
	ItemMap.Add(5, [](UActionBase* Context)
	{
		UItemLogicLibrary::WallPotion_Logic(Cast<UItem_Action>(Context));
	});
	ItemMap.Add(6, [](UActionBase* Context)
	{
		UItemLogicLibrary::EverwherePotion_Logic(Cast<UItem_Action>(Context));
	});
}
