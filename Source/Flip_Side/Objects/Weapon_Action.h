#pragma once

#include "CoreMinimal.h"
#include "Objects/ActionBase.h"
#include "DataTypes/CoinStatDataTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Weapon_Action.generated.h"

struct FRegisteredAbilityLogic;

/** 클릭 순간의 불변 스탯과 한 행동 동안의 가변 실행 상태를 함께 소유합니다. */
UCLASS()
class FLIP_SIDE_API UWeapon_Action : public UActionBase
{
	GENERATED_BODY()

public:
	bool InitializeAction(
		class ACoinActor* InCaster,
		const FWeaponActionSnapshot& InSnapshot,
		const FFaceData& InWeaponData
	);

	bool RefreshSnapshot(const FWeaponActionSnapshot& InSnapshot);
	void ResetAction();
	bool IsSnapshotCurrent() const;

	const FWeaponActionSnapshot& GetSnapshot() const { return ActionSnapshot; }
	FWeaponExecutionState& GetExecutionState() { return ExecutionState; }
	const FWeaponExecutionState& GetExecutionState() const { return ExecutionState; }

	int32 GetFinalAttackPoint() const { return ActionSnapshot.FinalNumericStats.AttackPoint; }
	int32 GetFinalBehaviorPoint() const { return ActionSnapshot.FinalNumericStats.WeaponPoint; }
	int32 GetFinalWeaponCount() const { return ActionSnapshot.FinalNumericStats.WeaponCnt; }

	void SetCasterCoin(ACoinActor* InCaster) { CasterCoin = InCaster; }
	ACoinActor* GetCasterCoin() const { return CasterCoin; }

	void SetAttackTargets(const TArray<FGridPoint>& InCells, class ABossActor* InBoss);
	void SetAbilityCells(const TArray<FGridPoint>& InCells) { AbilityCells = InCells; }
	void SetCurrentAbilityTargets(
		const TArray<ACoinActor*>& InCoins,
		const TArray<class ABase_OtherActor*>& InOthers,
		class AGridActor* InGrid
	);

	const TArray<FGridPoint>& GetAttackCells() const { return AttackCells; }
	const TArray<FGridPoint>& GetAbilityCells() const { return AbilityCells; }
	ABossActor* GetAttackBoss() const { return AttackBoss; }
	AGridActor* GetTargetGrid() const { return TargetGrid; }
	ABase_OtherActor* GetTargetOther() const { return TargetOther; }

	bool WasAbilityActorSelected(const AActor* Actor) const;
	void MarkAbilityActorSelected(AActor* Actor);

	const FFaceData& GetWeaponData() const { return WeaponData; }
	UTexture2D* GetWeaponIcon() const { return WeaponData.WeaponIcon; }

	FWeaponAttackResult ExecuteAttack();
	bool ExecuteAbility(const FRegisteredAbilityLogic& AbilityLogic);

	virtual void ExecuteAction() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Snapshot")
	FWeaponActionSnapshot ActionSnapshot;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Execution")
	FWeaponExecutionState ExecutionState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Presentation")
	FFaceData WeaponData;

	UPROPERTY()
	TObjectPtr<ACoinActor> CasterCoin = nullptr;

	UPROPERTY()
	TObjectPtr<ABossActor> AttackBoss = nullptr;

	UPROPERTY()
	TObjectPtr<AGridActor> TargetGrid = nullptr;

	UPROPERTY()
	TObjectPtr<ABase_OtherActor> TargetOther = nullptr;

	UPROPERTY()
	TArray<FGridPoint> AttackCells;

	UPROPERTY()
	TArray<FGridPoint> AbilityCells;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SelectedAbilityActors;
};
