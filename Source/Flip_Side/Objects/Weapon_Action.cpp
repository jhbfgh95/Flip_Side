#include "Objects/Weapon_Action.h"

#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
#include "Actors/Boss/BossActor.h"
#include "Actors/GridActor.h"
#include "Actors/Others/Base_OtherActor.h"
#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"

bool UWeapon_Action::InitializeAction(
	ACoinActor* InCaster,
	const FWeaponActionSnapshot& InSnapshot,
	const FFaceData& InWeaponData)
{
	ResetAction();
	if (!IsValid(InCaster) || !IsValid(InCaster->StatComponent) ||
		InSnapshot.WeaponID == INDEX_NONE || InSnapshot.WeaponID != InWeaponData.WeaponID)
	{
		return false;
	}

	CasterCoin = InCaster;
	ActionSnapshot = InSnapshot;
	WeaponData = InWeaponData;
	LogicID = InSnapshot.WeaponID;
	return true;
}

bool UWeapon_Action::RefreshSnapshot(const FWeaponActionSnapshot& InSnapshot)
{
	if (!IsValid(CasterCoin) || InSnapshot.WeaponID == INDEX_NONE ||
		InSnapshot.WeaponID != LogicID || InSnapshot.WeaponID != WeaponData.WeaponID)
	{
		return false;
	}

	ActionSnapshot = InSnapshot;
	return true;
}

void UWeapon_Action::ResetAction()
{
	ActionSnapshot = FWeaponActionSnapshot();
	ExecutionState = FWeaponExecutionState();
	WeaponData = FFaceData();
	CasterCoin = nullptr;
	AttackBoss = nullptr;
	TargetGrid = nullptr;
	TargetOther = nullptr;
	AttackCells.Reset();
	AbilityCells.Reset();
	SelectedAbilityActors.Reset();
	InRangeCoins.Reset();
	InRangeOthers.Reset();
	InRangeBoss = nullptr;
	LogicID = INDEX_NONE;
}

bool UWeapon_Action::IsSnapshotCurrent() const
{
	return IsValid(CasterCoin) && IsValid(CasterCoin->StatComponent) &&
		ActionSnapshot.SourceStatRevision == CasterCoin->StatComponent->GetWeaponStatRevision() &&
		ActionSnapshot.Face == CasterCoin->GetCoinDecidedFace() &&
		ActionSnapshot.WeaponID == CasterCoin->GetCoinFaceID();
}

void UWeapon_Action::SetAttackTargets(const TArray<FGridPoint>& InCells, ABossActor* InBoss)
{
	AttackCells = InCells;
	AttackBoss = IsValid(InBoss) ? InBoss : nullptr;
	InRangeBoss = AttackBoss;
}

void UWeapon_Action::SetCurrentAbilityTargets(
	const TArray<ACoinActor*>& InCoins,
	const TArray<ABase_OtherActor*>& InOthers,
	AGridActor* InGrid)
{
	InRangeCoins.Reset();
	for (ACoinActor* Coin : InCoins)
	{
		if (IsValid(Coin))
		{
			InRangeCoins.Add(Coin);
		}
	}

	InRangeOthers.Reset();
	for (ABase_OtherActor* Other : InOthers)
	{
		if (IsValid(Other))
		{
			InRangeOthers.Add(Other);
		}
	}

	TargetGrid = IsValid(InGrid) ? InGrid : nullptr;
	TargetOther = InRangeOthers.IsEmpty() ? nullptr : InRangeOthers[0];
}

bool UWeapon_Action::WasAbilityActorSelected(const AActor* Actor) const
{
	return IsValid(Actor) && SelectedAbilityActors.Contains(Actor);
}

void UWeapon_Action::MarkAbilityActorSelected(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	SelectedAbilityActors.AddUnique(Actor);
	if (const ACoinActor* Coin = Cast<ACoinActor>(Actor))
	{
		ExecutionState.SelectedCoinInstanceIDs.AddUnique(Coin->GetCoinID());
	}
}

FWeaponAttackResult UWeapon_Action::ExecuteAttack()
{
	FWeaponAttackResult Result;
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	UActionLogicRegistryGISubsystem* Registry = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UActionLogicRegistryGISubsystem>()
		: nullptr;
	const FWeaponLogicSet* LogicSet = IsValid(Registry) ? Registry->FindWeaponLogic(LogicID) : nullptr;
	if (!LogicSet || !LogicSet->AttackLogic)
	{
		return Result;
	}

	Result = LogicSet->AttackLogic(this);
	ExecutionState.LastAttack = Result;
	ExecutionState.TotalDamageDealt += Result.GetTotalDamage();
	return Result;
}

bool UWeapon_Action::ExecuteAbility(const FRegisteredAbilityLogic& AbilityLogic)
{
	return AbilityLogic.Logic ? AbilityLogic.Logic(this) : false;
}

void UWeapon_Action::ExecuteAction()
{
	ExecuteAttack();
}
