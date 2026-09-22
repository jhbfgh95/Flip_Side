#include "Actors/DebuffComponent.h"
#include "Engine/World.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"

UDebuffComponent::UDebuffComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UDebuffComponent::GetBattleTurn() const
{
	const UWorld* World = GetWorld();
	const UBattleManagerWSubsystem* Battle = IsValid(World) ? World->GetSubsystem<UBattleManagerWSubsystem>() : nullptr;
	return IsValid(Battle) ? Battle->GetTurnCount() : 0;
}

ECCTypes UDebuffComponent::GetCCType() const
{
	for (const FStatusEffectInstance& Effect : Effects)
	{
		if (Effect.CCType != ECCTypes::None) return Effect.CCType;
	}
	return ECCTypes::None;
}

void UDebuffComponent::Notify(const FStatusEffectInstance& Effect, bool bGameplayChanged, ECCTypes PreviousCC)
{
	OnDebuffChanged.Broadcast(Effect, bGameplayChanged);
	if (PreviousCC != GetCCType()) OnCCChanged.Broadcast(GetCCType());
}

bool UDebuffComponent::ApplyCC(ECCTypes Type, int32 Turns, EStatusEffectSourceType SourceType, int32 SourceDataID)
{
	if (Type != ECCTypes::Blind && Type != ECCTypes::Stun) return false;
	FStatusEffectInstance Effect;
	Effect.BuffTypeID = Type == ECCTypes::Blind ? DebuffTypeID::Blind : DebuffTypeID::Stun;
	Effect.Polarity = EStatusPolarity::Debuff;
	Effect.CCType = Type;
	Effect.RemainingTurns = Turns;
	Effect.SourceType = SourceType;
	Effect.SourceDataID = SourceDataID;
	return ApplyDebuff(Effect);
}

bool UDebuffComponent::IsSupportedEffect(const FStatusEffectInstance& Effect) const
{
	if (Effect.Polarity != EStatusPolarity::Debuff || Effect.BuffTypeID == INDEX_NONE || Effect.RemainingTurns <= 0)
		return false;
	if (Effect.CCType != ECCTypes::None && Effect.CCType != ECCTypes::Blind && Effect.CCType != ECCTypes::Stun)
		return false;
	// 이번 공통 범위는 공격력/무기력 약화와 실명/기절뿐입니다. 독과 반응형 버프는 받지 않습니다.
	const FWeaponStatModifier& M = Effect.Modifier;
	if (M.AttackPoint > 0 || M.WeaponPoint > 0 || M.WeaponCnt != 0 || M.MaxHP != 0 ||
		!M.AttackAreaSpec.IsZero() || !M.AbilityAreaSpec.IsZero() ||
		Effect.ReactiveBehavior != EStatusReactiveBehavior::None || (bAttackOnly && M.WeaponPoint != 0)) return false;
	if (Effect.CCType != ECCTypes::None && !M.IsZero()) return false;
	if (Effect.CCType == ECCTypes::None && M.IsZero()) return false;
	return true;
}

bool UDebuffComponent::ApplyDebuff(FStatusEffectInstance Effect)
{
	if (!bAcceptsEffects || !IsSupportedEffect(Effect)) return false;
	if (IsValid(GetOwner()) && GetOwner()->IsActorBeingDestroyed()) return false;
	const FWeaponStatModifier& M = Effect.Modifier;
	Effect.RemainingTurns = FMath::Clamp(Effect.RemainingTurns, 1, 999);
	if (Effect.CCType != ECCTypes::None && ConsumeIncomingCC.IsBound() && ConsumeIncomingCC.Execute(Effect)) return true;
	Effect.DurationType = EBuffDurationType::PersistentInBattle;
	Effect.StackPolicy = EStatusStackPolicy::NonStackable;
	Effect.LastDurationTurn = GetBattleTurn();
	const ECCTypes PreviousCC = GetCCType();
	const int32 Index = Effects.IndexOfByPredicate([&](const FStatusEffectInstance& Existing)
	{
		return Effect.CCType != ECCTypes::None ? Existing.CCType != ECCTypes::None : Existing.BuffTypeID == Effect.BuffTypeID;
	});
	if (Index != INDEX_NONE)
	{
		const FStatusEffectInstance Previous = Effects[Index];
		const bool bGameplayChanged = Previous.CCType != Effect.CCType ||
			Previous.Modifier.AttackPoint != M.AttackPoint || Previous.Modifier.WeaponPoint != M.WeaponPoint;
		Effect.BuffInstanceSerial = Previous.BuffInstanceSerial;
		Effects[Index] = Effect;
		// 이전 출처/종류 아이콘 제거도 같은 최종 상태를 기준으로 알립니다.
		if (Previous.BuffTypeID != Effect.BuffTypeID || Previous.SourceType != Effect.SourceType || Previous.SourceDataID != Effect.SourceDataID)
			OnDebuffChanged.Broadcast(Previous, false);
		Notify(Effect, bGameplayChanged, PreviousCC);
	}
	else
	{
		if (NextSerial == MIN_int32) return false;
		Effect.BuffInstanceSerial = NextSerial--;
		Effects.Add(Effect);
		Notify(Effect, true, PreviousCC);
	}
	return true;
}

bool UDebuffComponent::RemoveBySerial(int32 Serial)
{
	const int32 Index = Effects.IndexOfByPredicate([Serial](const FStatusEffectInstance& E) { return E.BuffInstanceSerial == Serial; });
	if (Index == INDEX_NONE) return false;
	const ECCTypes PreviousCC = GetCCType();
	const FStatusEffectInstance Removed = Effects[Index];
	Effects.RemoveAt(Index);
	Notify(Removed, true, PreviousCC);
	return true;
}

int32 UDebuffComponent::RemoveByTypeAndSource(int32 TypeID, EStatusEffectSourceType Source, int32 SourceID)
{
	int32 Count = 0;
	const TArray<FStatusEffectInstance> Copy = Effects;
	for (const FStatusEffectInstance& E : Copy)
		if (E.BuffTypeID == TypeID && E.SourceType == Source && E.SourceDataID == SourceID) Count += RemoveBySerial(E.BuffInstanceSerial);
	return Count;
}

void UDebuffComponent::ClearDebuffs()
{
	const TArray<FStatusEffectInstance> Copy = Effects;
	for (const FStatusEffectInstance& E : Copy) RemoveBySerial(E.BuffInstanceSerial);
}

void UDebuffComponent::ClearCC()
{
	const TArray<FStatusEffectInstance> Copy = Effects;
	for (const FStatusEffectInstance& E : Copy) if (E.CCType != ECCTypes::None) RemoveBySerial(E.BuffInstanceSerial);
}

void UDebuffComponent::DecreaseCCDuration(int32 Amount)
{
	if (Amount <= 0) return;
	for (FStatusEffectInstance& E : Effects)
	{
		if (E.CCType == ECCTypes::None) continue;
		E.RemainingTurns = FMath::Max(0, E.RemainingTurns - Amount);
		if (E.RemainingTurns == 0) RemoveBySerial(E.BuffInstanceSerial);
		else OnDebuffChanged.Broadcast(E, false);
		return;
	}
}

void UDebuffComponent::AdvanceTurnEnd(int32 CompletedTurn)
{
	// 적용 턴은 제외하며, 중복 종료 알림도 스냅숏 안의 턴 번호로 방지합니다.
	const TArray<FStatusEffectInstance> Copy = Effects;
	for (const FStatusEffectInstance& Old : Copy)
	{
		if (CompletedTurn <= Old.LastDurationTurn) continue;
		FStatusEffectInstance* E = Effects.FindByPredicate([&](const FStatusEffectInstance& V) { return V.BuffInstanceSerial == Old.BuffInstanceSerial; });
		if (!E) continue;
		E->RemainingTurns = FMath::Max(0, E->RemainingTurns - (CompletedTurn - E->LastDurationTurn));
		E->LastDurationTurn = CompletedTurn;
		if (E->RemainingTurns == 0) RemoveBySerial(E->BuffInstanceSerial);
		else OnDebuffChanged.Broadcast(*E, false);
	}
}

void UDebuffComponent::Restore(const TArray<FStatusEffectInstance>& SavedEffects)
{
	if (!bAcceptsEffects) return;
	ClearDebuffs();
	for (FStatusEffectInstance E : SavedEffects)
	{
		if (!IsSupportedEffect(E) || NextSerial == MIN_int32) continue;
		// 잘못된 저장 데이터도 동일 타입/CC를 중복 생성하지 않으며 마지막 항목을 사용합니다.
		const TArray<FStatusEffectInstance> Existing = Effects;
		for (const FStatusEffectInstance& Old : Existing)
			if ((E.CCType != ECCTypes::None && Old.CCType != ECCTypes::None) || Old.BuffTypeID == E.BuffTypeID)
				RemoveBySerial(Old.BuffInstanceSerial);
		// 복원은 재적용이 아니므로 갱신/그로기 전환 없이 기존 수명과 출처를 보존합니다.
		E.BuffInstanceSerial = NextSerial--;
		E.DurationType = EBuffDurationType::PersistentInBattle;
		E.RemainingTurns = FMath::Clamp(E.RemainingTurns, 1, 999);
		if (E.LastDurationTurn == INDEX_NONE) E.LastDurationTurn = GetBattleTurn();
		const ECCTypes PreviousCC = GetCCType();
		Effects.Add(E);
		Notify(E, true, PreviousCC);
	}
}

int32 UDebuffComponent::GetAttackModifier() const
{
	int64 Sum = 0;
	for (const FStatusEffectInstance& E : Effects) Sum += E.Modifier.AttackPoint;
	return static_cast<int32>(FMath::Clamp<int64>(Sum, MIN_int32, 0));
}
