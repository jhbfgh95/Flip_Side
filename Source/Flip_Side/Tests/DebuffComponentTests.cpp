#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Actors/DebuffComponent.h"
#include "Actors/Component_Status.h"
#include "Actors/CoinActor.h"
#include "Actors/Boss/BossActor_Groggy.h"
#include "BossPattern/BossGimmick_Groggy.h"
#include "BossPattern/BossGimmick_Blind.h"
#include "Objects/Weapon_Action.h"
#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"
#include "Engine/World.h"

namespace DebuffTests
{
	FStatusEffectInstance Weakness(int32 Turns = 2)
	{
		FStatusEffectInstance E;
		E.BuffTypeID = 12345;
		E.Polarity = EStatusPolarity::Debuff;
		E.RemainingTurns = Turns;
		E.Modifier.AttackPoint = -2;
		E.SourceType = EStatusEffectSourceType::Boss;
		E.SourceDataID = 1;
		return E;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffDurationTest, "FlipSide.Debuff.RefreshAndDuration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffDurationTest::RunTest(const FString& Parameters)
{
	UDebuffComponent* D = NewObject<UDebuffComponent>();
	int32 GameplayChanges = 0, UIChanges = 0;
	D->OnDebuffChanged.AddLambda([&](const FStatusEffectInstance&, bool Gameplay) { ++UIChanges; GameplayChanges += Gameplay; });
	TestTrue(TEXT("Apply numeric debuff"), D->ApplyDebuff(DebuffTests::Weakness()));
	TestEqual(TEXT("Numeric state changes once"), GameplayChanges, 1);
	D->AdvanceTurnEnd(0);
	TestEqual(TEXT("Application turn not consumed"), D->GetDebuffs()[0].RemainingTurns, 2);
	D->AdvanceTurnEnd(1);
	TestEqual(TEXT("Next completed turn consumes one"), D->GetDebuffs()[0].RemainingTurns, 1);
	D->AdvanceTurnEnd(1);
	TestEqual(TEXT("Duplicate turn notification ignored"), D->GetDebuffs()[0].RemainingTurns, 1);
	TestEqual(TEXT("Duration only does not change gameplay revision"), GameplayChanges, 1);
	TestEqual(TEXT("UI notified for duration"), UIChanges, 2);
	TestTrue(TEXT("Same debuff refresh accepted"), D->ApplyDebuff(DebuffTests::Weakness(3)));
	TestEqual(TEXT("No duplicate stack"), D->GetDebuffs().Num(), 1);
	TestEqual(TEXT("Refresh does not change modifier revision"), GameplayChanges, 1);
	TestEqual(TEXT("Refreshed duration"), D->GetDebuffs()[0].RemainingTurns, 3);
	D->AdvanceTurnEnd(3);
	TestFalse(TEXT("Expired debuff removed"), D->HasAnyDebuff());
	TestEqual(TEXT("Removal changes gameplay"), GameplayChanges, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffCCTest, "FlipSide.Debuff.CCReplacementAndValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffCCTest::RunTest(const FString& Parameters)
{
	UDebuffComponent* D = NewObject<UDebuffComponent>();
	TestFalse(TEXT("Poison excluded"), D->ApplyCC(ECCTypes::Poison, 3));
	TestFalse(TEXT("Unsupported sleep excluded"), D->ApplyCC(ECCTypes::Sleep, 3));
	TestFalse(TEXT("Zero duration rejected"), D->ApplyCC(ECCTypes::Stun, 0));
	TestTrue(TEXT("Blind accepted"), D->ApplyCC(ECCTypes::Blind, 5));
	TestTrue(TEXT("Stun replaces blind"), D->ApplyCC(ECCTypes::Stun, 1));
	TestEqual(TEXT("Only one CC"), D->GetDebuffs().Num(), 1);
	TestTrue(TEXT("Last CC wins"), D->GetCCType() == ECCTypes::Stun);
	D->AdvanceTurnEnd(1);
	TestTrue(TEXT("Previous blind does not resume"), D->GetCCType() == ECCTypes::None);
	D->bAttackOnly = true;
	FStatusEffectInstance E = DebuffTests::Weakness();
	E.Modifier.WeaponPoint = -1;
	TestFalse(TEXT("Boss rejects weapon power debuff"), D->ApplyDebuff(E));
	TestTrue(TEXT("Boss accepts attack debuff"), D->ApplyDebuff(DebuffTests::Weakness()));
	D->DisableForDeath();
	TestFalse(TEXT("Death clears debuffs"), D->HasAnyDebuff());
	TestFalse(TEXT("Dead owner cannot receive CC"), D->ApplyCC(ECCTypes::Blind, 2));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffPersistenceTest, "FlipSide.Debuff.Persistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffPersistenceTest::RunTest(const FString& Parameters)
{
	UDebuffComponent* D = NewObject<UDebuffComponent>();
	D->ApplyDebuff(DebuffTests::Weakness(3));
	D->ApplyCC(ECCTypes::Blind, 2, EStatusEffectSourceType::Coin, 7);
	D->AdvanceTurnEnd(1);
	UDebuffComponent* Restored = NewObject<UDebuffComponent>();
	Restored->Restore(D->GetDebuffs());
	TestEqual(TEXT("Stats and CC restored"), Restored->GetDebuffs().Num(), 2);
	TestTrue(TEXT("CC restored"), Restored->GetCCType() == ECCTypes::Blind);
	Restored->AdvanceTurnEnd(1);
	TestTrue(TEXT("Restoration does not decrement twice"), Restored->GetCCType() == ECCTypes::Blind);
	Restored->AdvanceTurnEnd(2);
	TestTrue(TEXT("CC expires at saved duration"), Restored->GetCCType() == ECCTypes::None);
	TestEqual(TEXT("Longer numeric effect remains"), Restored->GetDebuffs().Num(), 1);
	Restored->AdvanceTurnEnd(3);
	TestFalse(TEXT("All saved effects expire"), Restored->HasAnyDebuff());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffCoinIntegrationTest, "FlipSide.Debuff.CoinStatsAndSnapshot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffCoinIntegrationTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Test world"), World)) return false;
	ACoinActor* Coin = World->SpawnActor<ACoinActor>();
	if (!TestNotNull(TEXT("Coin"), Coin)) { World->DestroyWorld(false); return false; }
	FCoinStatInitializeData Init;
	Init.FrontWeaponStats.WeaponID = 1;
	Init.BackWeaponStats.WeaponID = 2;
	Init.FrontWeaponStats.BaseNumericStats.AttackPoint = 7;
	Init.BackWeaponStats.BaseNumericStats.AttackPoint = 9;
	Init.RuntimeState.BaseMaxHP = 10;
	Init.RuntimeState.CurrentHP = 8;
	UComponent_Status* Status = Coin->StatComponent;
	TestTrue(TEXT("Stats initialized"), Status->InitializeCoinStats(Init));
	const int32 InitialRevision = Status->GetWeaponStatRevision();
	Status->AddStatusEffect(DebuffTests::Weakness());
	TestEqual(TEXT("Front debuffed"), Status->ResolveFaceStats(EFaceState::Front).FinalNumericStats.AttackPoint, 5);
	TestEqual(TEXT("Back debuffed"), Status->ResolveFaceStats(EFaceState::Back).FinalNumericStats.AttackPoint, 7);
	TestTrue(TEXT("Modifier invalidates snapshot"), Status->GetWeaponStatRevision() > InitialRevision);
	const int32 NumericRevision = Status->GetWeaponStatRevision();
	Status->AddStatusEffect(DebuffTests::Weakness(3));
	TestEqual(TEXT("Refresh leaves stat revision intact"), Status->GetWeaponStatRevision(), NumericRevision);
	Coin->DebuffComponent->ApplyCC(ECCTypes::Blind, 2);
	TestTrue(TEXT("Blind allows action, suppresses hits"), Status->IsBlinded() && !Status->IsStunned());
	TestTrue(TEXT("CC invalidates snapshot"), Status->GetWeaponStatRevision() > NumericRevision);
	Init.RuntimeState = Status->ExportRuntimeState();
	ACoinActor* Restored = World->SpawnActor<ACoinActor>();
	if (TestNotNull(TEXT("Restored coin"), Restored))
	{
		TestTrue(TEXT("Actor recreation restores stats"), Restored->StatComponent->InitializeCoinStats(Init));
		TestEqual(TEXT("HP preserved"), Restored->StatComponent->GetHP(), 8);
		TestTrue(TEXT("Blind survives recreation"), Restored->StatComponent->IsBlinded());
		TestEqual(TEXT("No duplicate numeric modifiers"), Restored->StatComponent->ResolveFaceStats(EFaceState::Front).FinalNumericStats.AttackPoint, 5);
	}
	Status->SetHP(0, false);
	TestFalse(TEXT("Death clears CC"), Coin->DebuffComponent->HasAnyDebuff());
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffGroggyTest, "FlipSide.Debuff.GroggyInterception",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffGroggyTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("World"), World)) return false;
	ABossActor_Groggy* Boss = World->SpawnActor<ABossActor_Groggy>();
	if (!TestNotNull(TEXT("Groggy boss"), Boss)) { World->DestroyWorld(false); return false; }
	UBossGimmick_Groggy* Gimmick = NewObject<UBossGimmick_Groggy>(Boss);
	Boss->AddGimmick(Gimmick);
	Gimmick->OnBattleStart(Boss);
	TestTrue(TEXT("Direct component CC passes through gimmick"), Boss->DebuffComponent->ApplyCC(ECCTypes::Stun, 3));
	TestEqual(TEXT("CC duration becomes groggy points"), Gimmick->GetCurrentGroggy(), 3);
	TestFalse(TEXT("Absorbed CC is not active"), Boss->DebuffComponent->HasAnyDebuff());
	FCCStructure Blind;
	Blind.CCType = ECCTypes::Blind;
	Blind.CCDuration = 2;
	Boss->ApplyCC(Blind);
	TestEqual(TEXT("Legacy API uses same interception"), Gimmick->GetCurrentGroggy(), 5);
	TestTrue(TEXT("Groggy exception does not prevent acting"), Boss->ConsumeCCForBossPhase());
	World->DestroyWorld(false);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffBossDamageTest, "FlipSide.Debuff.BossBlindPreservesGimmick",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffBossDamageTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("World"), World)) return false;
	ABossActor* Boss = World->SpawnActor<ABossActor>();
	ACoinActor* Coin = World->SpawnActor<ACoinActor>();
	if (!TestNotNull(TEXT("Boss"), Boss) || !TestNotNull(TEXT("Coin"), Coin)) { World->DestroyWorld(false); return false; }
	Coin->StatComponent->SetHP(10, true);
	UBossPatternBase* Pattern = NewObject<UBossPatternBase>(Boss);
	Boss->SetActiveGimmick(NewObject<UBossGimmick_Blind>(Boss));
	FBossPhaseContext Context;
	Context.BaseDamage = 3;
	Context.CurrentPatternIndex = 0;
	Boss->DebuffComponent->ApplyCC(ECCTypes::Blind, 1);
	TestTrue(TEXT("Blind boss can act"), Boss->ConsumeCCForBossPhase());
	TestTrue(TEXT("Blind not expired before damage"), Boss->IsBlinded());
	Pattern->ExecutePattern(Boss, Context, { Coin }, {});
	TestEqual(TEXT("Blind prevents pattern damage"), Coin->StatComponent->GetHP(), 10);
	TestTrue(TEXT("Gimmick still applies coin blind"), Coin->StatComponent->IsBlinded());
	Boss->DebuffComponent->ClearCC();
	Pattern->ExecutePattern(Boss, Context, { Coin }, {});
	TestEqual(TEXT("Without blind pattern deals damage"), Coin->StatComponent->GetHP(), 7);
	Coin->DebuffComponent->ClearDebuffs();
	Boss->DebuffComponent->ApplyCC(ECCTypes::Stun, 1);
	TestFalse(TEXT("Stun prevents boss action"), Boss->ConsumeCCForBossPhase());
	Pattern->ExecutePattern(Boss, Context, { Coin }, {});
	TestFalse(TEXT("Stun prevents gimmick too"), Coin->StatComponent->GetOnIsOnCC());
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffCoinActionTest, "FlipSide.Debuff.CoinAbilityGating",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffCoinActionTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("World"), World)) return false;
	ACoinActor* Coin = World->SpawnActor<ACoinActor>();
	if (!TestNotNull(TEXT("Coin"), Coin)) { World->DestroyWorld(false); return false; }
	UWeapon_Action* Action = NewObject<UWeapon_Action>(Coin);
	Action->SetCasterCoin(Coin);
	int32 Calls = 0;
	FRegisteredAbilityLogic Ability;
	Ability.Logic = [&](UWeapon_Action*) { ++Calls; return true; };
	Coin->DebuffComponent->ApplyCC(ECCTypes::Blind, 1);
	Ability.Timing = EAbilityTiming::BeforeAttack;
	TestTrue(TEXT("Blind retains activation before attack"), Action->ExecuteAbility(Ability));
	Ability.Timing = EAbilityTiming::AfterAttackAlways;
	TestTrue(TEXT("Blind retains unconditional activation"), Action->ExecuteAbility(Ability));
	Ability.Timing = EAbilityTiming::OnHit;
	TestFalse(TEXT("Blind skips hit sub-effects"), Action->ExecuteAbility(Ability));
	TestEqual(TEXT("Only activation handlers ran"), Calls, 2);
	Coin->DebuffComponent->ApplyCC(ECCTypes::Stun, 1);
	Ability.Timing = EAbilityTiming::AfterAttackAlways;
	TestFalse(TEXT("Stun blocks activation"), Action->ExecuteAbility(Ability));
	TestEqual(TEXT("No handler ran while stunned"), Calls, 2);
	World->DestroyWorld(false);
	return true;
}
#endif
