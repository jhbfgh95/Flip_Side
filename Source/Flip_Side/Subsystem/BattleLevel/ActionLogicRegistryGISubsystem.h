#pragma once

#include "CoreMinimal.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActionLogicRegistryGISubsystem.generated.h"

using FActionLogic = TFunction<void(class UActionBase* Context)>;
using FAttackLogic = TFunction<FWeaponAttackResult(class UWeapon_Action* Context)>;
using FAbilityLogic = TFunction<bool(class UWeapon_Action* Context)>;

/** 한 무기에 여러 능력 타이밍을 등록할 수 있는 Registry 항목입니다. */
struct FRegisteredAbilityLogic
{
	EAbilityTiming Timing = EAbilityTiming::AfterAttackAlways;
	FAbilityTargetRule TargetRule;
	FAbilityLogic Logic;
	FName DebugName = NAME_None;
};

/** WeaponID 하나에 기본 공격과 복수 능력, [연속] 채널을 함께 보관합니다. */
struct FWeaponLogicSet
{
	FAttackLogic AttackLogic;
	TArray<FRegisteredAbilityLogic> AbilityLogics;
	ERepeatLogicChannel RepeatChannel = ERepeatLogicChannel::None;
	ERepeatCountSource AttackRepeatCountSource = ERepeatCountSource::One;
};

UCLASS()
class FLIP_SIDE_API UActionLogicRegistryGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RegisterAttackLogic(int32 ID, FAttackLogic Logic, ERepeatCountSource RepeatSource = ERepeatCountSource::One);
	void RegisterAbilityLogic(
		int32 ID,
		EAbilityTiming Timing,
		const FAbilityTargetRule& TargetRule,
		FAbilityLogic Logic,
		FName DebugName
	);

	const FWeaponLogicSet* FindWeaponLogic(int32 ID) const { return WeaponLogicMap.Find(ID); }

	// 아이템 리팩터링 전까지 기존 ItemAction 호출 규약을 유지합니다.
	void RegisterItemLogic(int32 ID, FActionLogic Logic) { ItemMap.Add(ID, MoveTemp(Logic)); }
	FActionLogic GetItemLogic(int32 ID) const { return ItemMap.FindRef(ID); }

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void RegisterAllWeaponLogic();
	void RegisterAllItemLogic();

	TMap<int32, FWeaponLogicSet> WeaponLogicMap;
	TMap<int32, FActionLogic> ItemMap;
};
