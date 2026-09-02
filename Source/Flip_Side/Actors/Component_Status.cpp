#include "Actors/Component_Status.h"
#include "DataTypes/WeaponDataTypes.h"

namespace
{
	EWeaponStatChangeFlags GetModifierChangeFlags(const FWeaponStatModifier& Modifier)
	{
		EWeaponStatChangeFlags Flags = EWeaponStatChangeFlags::None;
		if (Modifier.AttackPoint != 0 || Modifier.WeaponPoint != 0 || Modifier.WeaponCnt != 0)
		{
			Flags |= EWeaponStatChangeFlags::NumericStats;
		}
		if (!Modifier.AttackAreaSpec.IsZero())
		{
			Flags |= EWeaponStatChangeFlags::AttackAreaSpec;
		}
		if (!Modifier.AbilityAreaSpec.IsZero())
		{
			Flags |= EWeaponStatChangeFlags::AbilityAreaSpec;
		}
		return Flags;
	}

	void AddScaledValue(int32& Target, int32 Value, int32 StackCount)
	{
		const int64 Result = static_cast<int64>(Target) + static_cast<int64>(Value) * StackCount;
		Target = static_cast<int32>(FMath::Clamp<int64>(Result, MIN_int32, MAX_int32));
	}
}

UComponent_Status::UComponent_Status()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComponent_Status::BeginPlay()
{
	Super::BeginPlay();
}

bool UComponent_Status::InitializeCoinStats(const FCoinStatInitializeData& InitializeData)
{
	if (bHasRuntimeInitialized ||
		InitializeData.FrontWeaponStats.WeaponID == INDEX_NONE ||
		InitializeData.BackWeaponStats.WeaponID == INDEX_NONE ||
		InitializeData.RuntimeState.BaseMaxHP <= 0 ||
		InitializeData.RuntimeState.CurrentHP <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[StatusComponent] 코인 스탯 초기화 실패: 무기 ID 또는 HP가 유효하지 않습니다."));
		return false;
	}

	FrontWeaponStats = InitializeData.FrontWeaponStats;
	BackWeaponStats = InitializeData.BackWeaponStats;
	FrontWeaponStats.BaseNumericStats.WeaponCnt = FMath::Max(0, FrontWeaponStats.BaseNumericStats.WeaponCnt);
	BackWeaponStats.BaseNumericStats.WeaponCnt = FMath::Max(0, BackWeaponStats.BaseNumericStats.WeaponCnt);

	BaseMaxHP = InitializeData.RuntimeState.BaseMaxHP;
	MaxHP = BaseMaxHP;
	Shield = FMath::Clamp(InitializeData.RuntimeState.Shield, 0, MAX_SHIELD);
	bIsDead = false;
	bHasRuntimeInitialized = true;
	CurrentFace = EFaceState::None;
	CurrentFaceIndex = 0;
	WeaponStatRevision = 0;
	NextBuffInstanceSerial = 1;
	ActiveStatusEffects.Reset();

	SetFaceWeaponStats(
		FrontWeaponStats.BaseNumericStats.WeaponPoint,
		FrontWeaponStats.BaseNumericStats.AttackPoint,
		BackWeaponStats.BaseNumericStats.WeaponPoint,
		BackWeaponStats.BaseNumericStats.AttackPoint
	);

	for (FStatusEffectInstance StatusEffect : InitializeData.RuntimeState.PersistentStatusEffects)
	{
		if (StatusEffect.BuffTypeID == INDEX_NONE ||
			StatusEffect.DurationType != EBuffDurationType::PersistentInBattle)
		{
			continue;
		}

		StatusEffect.BuffInstanceSerial = NextBuffInstanceSerial++;
		if (StatusEffect.Polarity == EStatusPolarity::Buff)
		{
			StatusEffect.StackPolicy = EStatusStackPolicy::Stack;
		}
		ActiveStatusEffects.Add(MoveTemp(StatusEffect));
	}

	RecalculateMaxHPFromEffects(false);
	HP = FMath::Clamp(InitializeData.RuntimeState.CurrentHP, 1, MaxHP);
	MarkWeaponStatsDirty(EWeaponStatChangeFlags::All);
	return true;
}

const FWeaponFaceStats* UComponent_Status::FindFaceStats(EFaceState Face) const
{
	if (Face == EFaceState::Front)
	{
		return &FrontWeaponStats;
	}
	if (Face == EFaceState::Back)
	{
		return &BackWeaponStats;
	}
	return nullptr;
}

FResolvedWeaponFaceStats UComponent_Status::ResolveFaceStats(EFaceState Face) const
{
	const EFaceState ResolvedFace = Face == EFaceState::None ? CurrentFace : Face;
	const FWeaponFaceStats* FaceStats = FindFaceStats(ResolvedFace);
	if (!FaceStats)
	{
		return FResolvedWeaponFaceStats();
	}
	return ResolveFaceStatsFromData(*FaceStats, ActiveStatusEffects);
}

FResolvedWeaponFaceStats UComponent_Status::ResolveFaceStatsFromData(
	const FWeaponFaceStats& FaceStats,
	const TArray<FStatusEffectInstance>& StatusEffects)
{
	FResolvedWeaponFaceStats ResolvedStats;
	ResolvedStats.WeaponID = FaceStats.WeaponID;
	ResolvedStats.BaseNumericStats = FaceStats.BaseNumericStats;
	ResolvedStats.FinalNumericStats = FaceStats.BaseNumericStats;
	ResolvedStats.AttackAreaSpec = FaceStats.AttackAreaSpec;
	ResolvedStats.AbilityAreaSpec = FaceStats.AbilityAreaSpec;
	ResolvedStats.bHasAbilityArea = FaceStats.bHasAbilityArea;

	for (const FStatusEffectInstance& StatusEffect : StatusEffects)
	{
		AddScaledValue(ResolvedStats.AppliedModifier.AttackPoint, StatusEffect.Modifier.AttackPoint, 1);
		AddScaledValue(ResolvedStats.AppliedModifier.WeaponPoint, StatusEffect.Modifier.WeaponPoint, 1);
		AddScaledValue(ResolvedStats.AppliedModifier.WeaponCnt, StatusEffect.Modifier.WeaponCnt, 1);
		AddScaledValue(ResolvedStats.AppliedModifier.AttackAreaSpec.ParamA, StatusEffect.Modifier.AttackAreaSpec.ParamA, 1);
		AddScaledValue(ResolvedStats.AppliedModifier.AttackAreaSpec.ParamB, StatusEffect.Modifier.AttackAreaSpec.ParamB, 1);
		AddScaledValue(ResolvedStats.AppliedModifier.AbilityAreaSpec.ParamA, StatusEffect.Modifier.AbilityAreaSpec.ParamA, 1);
		AddScaledValue(ResolvedStats.AppliedModifier.AbilityAreaSpec.ParamB, StatusEffect.Modifier.AbilityAreaSpec.ParamB, 1);
	}

	AddScaledValue(ResolvedStats.FinalNumericStats.AttackPoint, ResolvedStats.AppliedModifier.AttackPoint, 1);
	AddScaledValue(ResolvedStats.FinalNumericStats.WeaponPoint, ResolvedStats.AppliedModifier.WeaponPoint, 1);
	AddScaledValue(ResolvedStats.FinalNumericStats.WeaponCnt, ResolvedStats.AppliedModifier.WeaponCnt, 1);
	ResolvedStats.FinalNumericStats.AttackPoint = FMath::Max(0, ResolvedStats.FinalNumericStats.AttackPoint);
	ResolvedStats.FinalNumericStats.WeaponPoint = FMath::Max(0, ResolvedStats.FinalNumericStats.WeaponPoint);
	ResolvedStats.FinalNumericStats.WeaponCnt = FMath::Max(0, ResolvedStats.FinalNumericStats.WeaponCnt);

	AddScaledValue(ResolvedStats.AttackAreaSpec.ParamA, ResolvedStats.AppliedModifier.AttackAreaSpec.ParamA, 1);
	AddScaledValue(ResolvedStats.AttackAreaSpec.ParamB, ResolvedStats.AppliedModifier.AttackAreaSpec.ParamB, 1);
	AddScaledValue(ResolvedStats.AbilityAreaSpec.ParamA, ResolvedStats.AppliedModifier.AbilityAreaSpec.ParamA, 1);
	AddScaledValue(ResolvedStats.AbilityAreaSpec.ParamB, ResolvedStats.AppliedModifier.AbilityAreaSpec.ParamB, 1);
	ResolvedStats.AttackAreaSpec.ParamA = FMath::Max(0, ResolvedStats.AttackAreaSpec.ParamA);
	ResolvedStats.AttackAreaSpec.ParamB = FMath::Max(0, ResolvedStats.AttackAreaSpec.ParamB);
	ResolvedStats.AbilityAreaSpec.ParamA = FMath::Max(0, ResolvedStats.AbilityAreaSpec.ParamA);
	ResolvedStats.AbilityAreaSpec.ParamB = FMath::Max(0, ResolvedStats.AbilityAreaSpec.ParamB);

	return ResolvedStats;
}

FWeaponActionSnapshot UComponent_Status::BuildActionSnapshot(EFaceState Face) const
{
	FWeaponActionSnapshot Snapshot;
	Snapshot.Face = Face == EFaceState::None ? CurrentFace : Face;

	const FResolvedWeaponFaceStats ResolvedStats = ResolveFaceStats(Snapshot.Face);
	Snapshot.WeaponID = ResolvedStats.WeaponID;
	Snapshot.FinalNumericStats = ResolvedStats.FinalNumericStats;
	Snapshot.AttackAreaSpec = ResolvedStats.AttackAreaSpec;
	Snapshot.AbilityAreaSpec = ResolvedStats.AbilityAreaSpec;
	Snapshot.bHasAbilityArea = ResolvedStats.bHasAbilityArea;
	Snapshot.SourceStatRevision = WeaponStatRevision;
	return Snapshot;
}

bool UComponent_Status::AddStatusEffect(FStatusEffectInstance StatusEffect)
{
	if (StatusEffect.BuffTypeID == INDEX_NONE)
	{
		return false;
	}

	if (StatusEffect.Polarity == EStatusPolarity::Buff)
	{
		StatusEffect.StackPolicy = EStatusStackPolicy::Stack;
	}
	else if (StatusEffect.StackPolicy == EStatusStackPolicy::NonStackable)
	{
		const bool bAlreadyApplied = ActiveStatusEffects.ContainsByPredicate([&StatusEffect](const FStatusEffectInstance& ActiveEffect)
		{
			return ActiveEffect.BuffTypeID == StatusEffect.BuffTypeID &&
				ActiveEffect.Polarity == StatusEffect.Polarity;
		});
		if (bAlreadyApplied)
		{
			return false;
		}
	}

	if (StatusEffect.ReactiveBehavior == EStatusReactiveBehavior::TemporaryShield)
	{
		const int32 PreviousShield = Shield;
		Shield = FMath::Clamp(Shield + FMath::Max(0, StatusEffect.ReactiveMagnitude), 0, MAX_SHIELD);
		StatusEffect.RuntimeValue = Shield - PreviousShield;
		if (StatusEffect.RuntimeValue <= 0)
		{
			return false;
		}
		OnShieldChanged.Broadcast(StatusEffect.RuntimeValue);
	}

	if (NextBuffInstanceSerial <= 0)
	{
		NextBuffInstanceSerial = 1;
	}
	StatusEffect.BuffInstanceSerial = NextBuffInstanceSerial++;

	const EWeaponStatChangeFlags ChangeFlags = GetModifierChangeFlags(StatusEffect.Modifier);
	ActiveStatusEffects.Add(StatusEffect);
	RecalculateMaxHPFromEffects(true);

	if (ChangeFlags != EWeaponStatChangeFlags::None)
	{
		MarkWeaponStatsDirty(ChangeFlags);
	}
	BroadcastStatusEffectChanged(StatusEffect);
	return true;
}

bool UComponent_Status::RemoveStatusEffectByInstanceSerial(int32 BuffInstanceSerial)
{
	const int32 EffectIndex = ActiveStatusEffects.IndexOfByPredicate([BuffInstanceSerial](const FStatusEffectInstance& StatusEffect)
	{
		return StatusEffect.BuffInstanceSerial == BuffInstanceSerial;
	});
	if (EffectIndex == INDEX_NONE)
	{
		return false;
	}

	return RemoveStatusEffectAtIndex(EffectIndex);
}

int32 UComponent_Status::RemoveTurnOnlyStatusEffects()
{
	int32 RemovedCount = 0;
	for (int32 EffectIndex = ActiveStatusEffects.Num() - 1; EffectIndex >= 0; --EffectIndex)
	{
		if (ActiveStatusEffects[EffectIndex].DurationType != EBuffDurationType::TurnOnly)
		{
			continue;
		}

		RemovedCount += RemoveStatusEffectAtIndex(EffectIndex) ? 1 : 0;
	}
	return RemovedCount;
}

int32 UComponent_Status::RemoveStatusEffectsByTypeAndSource(
	int32 BuffTypeID,
	EStatusEffectSourceType SourceType,
	int32 SourceDataID)
{
	int32 RemovedCount = 0;
	for (int32 EffectIndex = ActiveStatusEffects.Num() - 1; EffectIndex >= 0; --EffectIndex)
	{
		const FStatusEffectInstance& Effect = ActiveStatusEffects[EffectIndex];
		if (Effect.BuffTypeID == BuffTypeID && Effect.SourceType == SourceType &&
			Effect.SourceDataID == SourceDataID)
		{
			RemovedCount += RemoveStatusEffectAtIndex(EffectIndex) ? 1 : 0;
		}
	}
	return RemovedCount;
}

int32 UComponent_Status::GetStatusEffectStackCount(
	int32 BuffTypeID,
	EStatusEffectSourceType SourceType,
	int32 SourceDataID) const
{
	int32 StackCount = 0;
	for (const FStatusEffectInstance& Effect : ActiveStatusEffects)
	{
		if (Effect.BuffTypeID == BuffTypeID && Effect.SourceType == SourceType &&
			Effect.SourceDataID == SourceDataID)
		{
			++StackCount;
		}
	}
	return StackCount;
}

int32 UComponent_Status::AdvancePersistentStatusEffectsAtTurnEnd()
{
	int32 RemovedCount = 0;
	for (int32 EffectIndex = ActiveStatusEffects.Num() - 1; EffectIndex >= 0; --EffectIndex)
	{
		FStatusEffectInstance& Effect = ActiveStatusEffects[EffectIndex];
		if (Effect.DurationType != EBuffDurationType::PersistentInBattle || Effect.RemainingTurns <= 0)
		{
			continue;
		}

		--Effect.RemainingTurns;
		if (Effect.RemainingTurns <= 0)
		{
			RemovedCount += RemoveStatusEffectAtIndex(EffectIndex) ? 1 : 0;
		}
	}
	return RemovedCount;
}

FCoinRuntimeStateSnapshot UComponent_Status::ExportRuntimeState() const
{
	FCoinRuntimeStateSnapshot RuntimeState;
	RuntimeState.BaseMaxHP = BaseMaxHP;
	RuntimeState.CurrentHP = HP;
	RuntimeState.Shield = Shield;
	RuntimeState.PersistentStatusEffects.Reserve(ActiveStatusEffects.Num());
	for (const FStatusEffectInstance& StatusEffect : ActiveStatusEffects)
	{
		if (StatusEffect.DurationType == EBuffDurationType::PersistentInBattle)
		{
			RuntimeState.PersistentStatusEffects.Add(StatusEffect);
		}
	}
	return RuntimeState;
}

bool UComponent_Status::ImportRuntimeState(const FCoinRuntimeStateSnapshot& RuntimeState)
{
	if (bIsDead || RuntimeState.BaseMaxHP <= 0 || RuntimeState.CurrentHP <= 0)
	{
		return false;
	}

	BaseMaxHP = RuntimeState.BaseMaxHP;
	MaxHP = BaseMaxHP;
	Shield = FMath::Clamp(RuntimeState.Shield, 0, MAX_SHIELD);
	ActiveStatusEffects.Reset();
	NextBuffInstanceSerial = 1;

	EWeaponStatChangeFlags ChangeFlags = EWeaponStatChangeFlags::None;
	for (FStatusEffectInstance StatusEffect : RuntimeState.PersistentStatusEffects)
	{
		if (StatusEffect.BuffTypeID == INDEX_NONE ||
			StatusEffect.DurationType != EBuffDurationType::PersistentInBattle)
		{
			continue;
		}

		StatusEffect.BuffInstanceSerial = NextBuffInstanceSerial++;
		if (StatusEffect.Polarity == EStatusPolarity::Buff)
		{
			StatusEffect.StackPolicy = EStatusStackPolicy::Stack;
		}
		ChangeFlags |= GetModifierChangeFlags(StatusEffect.Modifier);
		ActiveStatusEffects.Add(MoveTemp(StatusEffect));
	}

	RecalculateMaxHPFromEffects(false);
	HP = FMath::Clamp(RuntimeState.CurrentHP, 1, MaxHP);
	if (ChangeFlags != EWeaponStatChangeFlags::None)
	{
		MarkWeaponStatsDirty(ChangeFlags);
	}
	RefreshStatusEffectEvents();
	return true;
}

void UComponent_Status::RefreshStatusEffectEvents() const
{
	TArray<FStatusEffectInstance> BroadcastedEffects;
	for (const FStatusEffectInstance& StatusEffect : ActiveStatusEffects)
	{
		const bool bAlreadyBroadcast = BroadcastedEffects.ContainsByPredicate([&StatusEffect](const FStatusEffectInstance& Effect)
		{
			return Effect.BuffTypeID == StatusEffect.BuffTypeID &&
				Effect.SourceType == StatusEffect.SourceType &&
				Effect.SourceDataID == StatusEffect.SourceDataID &&
				Effect.Polarity == StatusEffect.Polarity;
		});
		if (!bAlreadyBroadcast)
		{
			BroadcastedEffects.Add(StatusEffect);
			BroadcastStatusEffectChanged(StatusEffect);
		}
	}
}

void UComponent_Status::MarkWeaponStatsDirty(EWeaponStatChangeFlags ChangeFlags)
{
	if (ChangeFlags == EWeaponStatChangeFlags::None)
	{
		return;
	}

	WeaponStatRevision = WeaponStatRevision == MAX_int32 ? 1 : WeaponStatRevision + 1;
	FWeaponStatsChangedEvent ChangedEvent;
	ChangedEvent.Revision = WeaponStatRevision;
	ChangedEvent.ChangeFlags = ChangeFlags;
	OnWeaponStatsChanged.Broadcast(ChangedEvent);
}

int32 UComponent_Status::CountStatusEffectStacks(const FStatusEffectInstance& StatusEffect) const
{
	int32 TotalStackCount = 0;
	for (const FStatusEffectInstance& ActiveEffect : ActiveStatusEffects)
	{
		if (ActiveEffect.BuffTypeID == StatusEffect.BuffTypeID &&
			ActiveEffect.SourceType == StatusEffect.SourceType &&
			ActiveEffect.SourceDataID == StatusEffect.SourceDataID &&
			ActiveEffect.Polarity == StatusEffect.Polarity)
		{
			++TotalStackCount;
		}
	}
	return TotalStackCount;
}

void UComponent_Status::BroadcastStatusEffectChanged(const FStatusEffectInstance& StatusEffect) const
{
	FStatusEffectsChangedEvent ChangedEvent;
	ChangedEvent.BuffTypeID = StatusEffect.BuffTypeID;
	ChangedEvent.SourceType = StatusEffect.SourceType;
	ChangedEvent.SourceDataID = StatusEffect.SourceDataID;
	ChangedEvent.TotalStackCount = CountStatusEffectStacks(StatusEffect);
	ChangedEvent.bIsDebuff = StatusEffect.Polarity == EStatusPolarity::Debuff;
	ChangedEvent.bIsActive = ChangedEvent.TotalStackCount > 0;
	OnStatusEffectsChanged.Broadcast(ChangedEvent);
}

void UComponent_Status::RecalculateMaxHPFromEffects(bool bIncreaseCurrentHPForPositiveDelta)
{
	int64 MaxHPModifier = 0;
	for (const FStatusEffectInstance& StatusEffect : ActiveStatusEffects)
	{
		MaxHPModifier += StatusEffect.Modifier.MaxHP;
	}

	const int32 PreviousMaxHP = MaxHP;
	const int64 CalculatedMaxHP = static_cast<int64>(BaseMaxHP) + MaxHPModifier;
	MaxHP = static_cast<int32>(FMath::Clamp<int64>(CalculatedMaxHP, 1, MAX_int32));
	const int32 MaxHPDelta = MaxHP - PreviousMaxHP;

	if (MaxHPDelta != 0)
	{
		OnMaxHPChanged.Broadcast(MaxHPDelta);
	}

	const int32 PreviousHP = HP;
	if (MaxHPDelta > 0 && bIncreaseCurrentHPForPositiveDelta)
	{
		HP = FMath::Clamp(HP + MaxHPDelta, 0, MaxHP);
	}
	else
	{
		HP = FMath::Clamp(HP, 0, MaxHP);
	}

	const int32 HPDelta = HP - PreviousHP;
	if (HPDelta != 0)
	{
		OnHpChanged.Broadcast(HPDelta);
	}
	HandleDeathIfNeeded();
}

bool UComponent_Status::RemoveStatusEffectAtIndex(int32 EffectIndex)
{
	if (!ActiveStatusEffects.IsValidIndex(EffectIndex))
	{
		return false;
	}

	const FStatusEffectInstance RemovedEffect = ActiveStatusEffects[EffectIndex];
	if (RemovedEffect.ReactiveBehavior == EStatusReactiveBehavior::TemporaryShield &&
		RemovedEffect.RuntimeValue > 0 && Shield > 0)
	{
		const int32 RemovedShield = FMath::Min(Shield, RemovedEffect.RuntimeValue);
		Shield -= RemovedShield;
		OnShieldChanged.Broadcast(-RemovedShield);
	}

	ActiveStatusEffects.RemoveAt(EffectIndex);
	RecalculateMaxHPFromEffects(false);
	const EWeaponStatChangeFlags ChangeFlags = GetModifierChangeFlags(RemovedEffect.Modifier);
	if (ChangeFlags != EWeaponStatChangeFlags::None)
	{
		MarkWeaponStatsDirty(ChangeFlags);
	}
	BroadcastStatusEffectChanged(RemovedEffect);
	return true;
}

void UComponent_Status::ConsumeTemporaryShieldContribution(int32 AbsorbedDamage)
{
	int32 RemainingAbsorbedDamage = FMath::Max(0, AbsorbedDamage);
	for (FStatusEffectInstance& Effect : ActiveStatusEffects)
	{
		if (RemainingAbsorbedDamage <= 0)
		{
			break;
		}
		if (Effect.ReactiveBehavior != EStatusReactiveBehavior::TemporaryShield || Effect.RuntimeValue <= 0)
		{
			continue;
		}

		const int32 ConsumedValue = FMath::Min(Effect.RuntimeValue, RemainingAbsorbedDamage);
		Effect.RuntimeValue -= ConsumedValue;
		RemainingAbsorbedDamage -= ConsumedValue;
	}
}

FActionTask UComponent_Status::GetModifiedStats()
{
	FActionTask FinalTask;
	FinalTask.ModifiedAttackPoint = 0;
	FinalTask.ModifiedBehaviorPoint = 0;
	FinalTask.ModifiedRange = FGridPoint(0, 0);

	if (OnCalculateStats.IsBound())
	{
		OnCalculateStats.Broadcast(FinalTask);
	}
	return FinalTask;
}

void UComponent_Status::SetHP(int32 ApplyHP, bool bIsFirst)
{
	if (bIsDead)
	{
		return;
	}

	if (bIsFirst)
	{
		if (ApplyHP <= 0)
		{
			return;
		}
		BaseMaxHP = ApplyHP;
		MaxHP = ApplyHP;
		HP = ApplyHP;
		return;
	}

	const int32 PreviousHP = HP;
	HP = FMath::Clamp(ApplyHP, 0, MaxHP);
	const int32 DeltaHP = HP - PreviousHP;
	if (DeltaHP != 0)
	{
		OnHpChanged.Broadcast(DeltaHP);
	}
	HandleDeathIfNeeded();
}

void UComponent_Status::SetFaceWeaponStats(int32 FrontBP, int32 FrontAP, int32 BackBP, int32 BackAP)
{
	BP.SetNum(2);
	AP.SetNum(2);
	BP[0] = FrontBP;
	AP[0] = FrontAP;
	BP[1] = BackBP;
	AP[1] = BackAP;
}

void UComponent_Status::ApplyFaceWeaponStat(EFaceState Face)
{
	if (Face != EFaceState::Front && Face != EFaceState::Back)
	{
		return;
	}

	const bool bFaceChanged = CurrentFace != Face;
	CurrentFace = Face;
	CurrentFaceIndex = Face == EFaceState::Front ? 0 : 1;
	if (bFaceChanged)
	{
		MarkWeaponStatsDirty(EWeaponStatChangeFlags::CurrentFace);
	}
}

void UComponent_Status::ApplyDamage(int32 Damage, AActor* DamageCauser)
{
	if (bIsDead || Damage <= 0)
	{
		return;
	}

	int32 FinalDamage = Damage;
	bool bIsIgnored = false;
	if (OnPreTakeDamage.IsBound())
	{
		OnPreTakeDamage.Broadcast(Damage, FinalDamage, bIsIgnored);
	}
	if (bIsIgnored)
	{
		return;
	}


	TArray<FStatusEffectInstance> TriggeredArmorEffects;
	TArray<int32> ConsumedEffectSerials;
	for (const FStatusEffectInstance& Effect : ActiveStatusEffects)
	{
		switch (Effect.ReactiveBehavior)
		{
		case EStatusReactiveBehavior::DodgeChance:
			if (FMath::RandRange(1, 100) <= FMath::Clamp(Effect.ReactiveMagnitude, 0, 100))
			{
				return;
			}
			break;
		case EStatusReactiveBehavior::ReduceNextDamageAndGrantAttack:
			FinalDamage = FMath::Max(0, FinalDamage - FMath::Max(0, Effect.ReactiveMagnitude));
			TriggeredArmorEffects.Add(Effect);
			if (Effect.RemainingTriggers > 0)
			{
				ConsumedEffectSerials.AddUnique(Effect.BuffInstanceSerial);
			}
			break;
		default:
			break;
		}
	}

	FinalDamage = FMath::Max(0, FinalDamage);
	int32 AbsorbedDamage = 0;
	if (Shield > 0 && FinalDamage > 0)
	{
		AbsorbedDamage = FMath::Min(Shield, FinalDamage);
		Shield -= AbsorbedDamage;
		FinalDamage -= AbsorbedDamage;
		ConsumeTemporaryShieldContribution(AbsorbedDamage);
		OnShieldChanged.Broadcast(-AbsorbedDamage);
	}

	for (const FStatusEffectInstance& Effect : ActiveStatusEffects)
	{
		if (Effect.ReactiveBehavior == EStatusReactiveBehavior::SurviveLethalOnce &&
			FinalDamage >= HP && HP > 0)
		{
			FinalDamage = FMath::Max(0, HP - 1);
			ConsumedEffectSerials.AddUnique(Effect.BuffInstanceSerial);
			break;
		}
	}

	const int32 PreviousHP = HP;
	if (FinalDamage > 0)
	{
		HPChanged(FinalDamage);
	}
	const int32 ActualHPDamage = FMath::Max(0, PreviousHP - HP);
	const bool bTookDamage = AbsorbedDamage + ActualHPDamage > 0;

	for (int32 BuffInstanceSerial : ConsumedEffectSerials)
	{
		RemoveStatusEffectByInstanceSerial(BuffInstanceSerial);
	}

	if (bTookDamage)
	{
		for (const FStatusEffectInstance& ArmorEffect : TriggeredArmorEffects)
		{
			const int32 NextTurnAttack = FMath::Max(0, FMath::RoundToInt(Damage * 0.5f));
			if (NextTurnAttack <= 0)
			{
				continue;
			}

			FStatusEffectInstance AttackBuff;
			AttackBuff.BuffTypeID = WeaponBuffTypeID::ArmorNextTurnAttack;
			AttackBuff.SourceType = ArmorEffect.SourceType;
			AttackBuff.SourceDataID = ArmorEffect.SourceDataID;
			AttackBuff.Polarity = EStatusPolarity::Buff;
			AttackBuff.DurationType = EBuffDurationType::PersistentInBattle;
			// 현재 BossPhase 종료와 다음 턴 종료를 통과한 뒤 제거됩니다.
			AttackBuff.RemainingTurns = 2;
			AttackBuff.Modifier.AttackPoint = NextTurnAttack;
			AddStatusEffect(AttackBuff);
		}
	}
}

void UComponent_Status::HPChanged(int32 Damage)
{
	if (bIsDead || Damage <= 0)
	{
		return;
	}

	const int32 PreviousHP = HP;
	HP = FMath::Clamp(HP - Damage, 0, MaxHP);
	const int32 ActualDamage = PreviousHP - HP;
	if (ActualDamage > 0)
	{
		OnHpChanged.Broadcast(-ActualDamage);
	}
	HandleDeathIfNeeded();
}

void UComponent_Status::HandleDeathIfNeeded()
{
	if (bIsDead || HP > 0)
	{
		return;
	}

	bIsDead = true;
	HP = 0;
	OnDead.Broadcast();
}

void UComponent_Status::ApplyHeal(int32 Heal, AActor* HealCauser)
{
	if (bIsDead || Heal <= 0)
	{
		return;
	}

	const int32 PreviousHP = HP;
	HP = FMath::Clamp(HP + Heal, 0, MaxHP);
	const int32 ActualHeal = HP - PreviousHP;
	if (ActualHeal > 0)
	{
		OnHpChanged.Broadcast(ActualHeal);
	}
}

void UComponent_Status::ApplyShield(int32 AddShield, AActor* ShieldCauser)
{
	if (bIsDead || AddShield == 0)
	{
		return;
	}

	const int32 PreviousShield = Shield;
	Shield = FMath::Clamp(Shield + AddShield, 0, MAX_SHIELD);
	const int32 DeltaShield = Shield - PreviousShield;
	if (DeltaShield != 0)
	{
		OnShieldChanged.Broadcast(DeltaShield);
	}
}

void UComponent_Status::AddBuffs(const FBuffInfo& Info)
{
	FBuffInfo NewBuff = Info;
	if (Info.StatDelegate.IsBound())
	{
		NewBuff.StatHandle = OnCalculateStats.Add(Info.StatDelegate);
	}
	if (Info.DamageDelegate.IsBound())
	{
		NewBuff.DamageHandle = OnPreTakeDamage.Add(Info.DamageDelegate);
	}
	if (Info.PreGiveDelegate.IsBound())
	{
		NewBuff.PreGiveHandle = OnPreGiveDamage.Add(Info.PreGiveDelegate);
	}
	if (Info.PostGiveDelegate.IsBound())
	{
		NewBuff.PostGiveHandle = OnPostGiveDamage.Add(Info.PostGiveDelegate);
	}

	ActiveBuffs.Add(NewBuff);
	OnBuffListChanged.Broadcast(true);
}

void UComponent_Status::RemoveLegacyBuffAt(int32 Index)
{
	if (!ActiveBuffs.IsValidIndex(Index))
	{
		return;
	}

	const FBuffInfo& Buff = ActiveBuffs[Index];
	if (Buff.StatHandle.IsValid())
	{
		OnCalculateStats.Remove(Buff.StatHandle);
	}
	if (Buff.DamageHandle.IsValid())
	{
		OnPreTakeDamage.Remove(Buff.DamageHandle);
	}
	if (Buff.PreGiveHandle.IsValid())
	{
		OnPreGiveDamage.Remove(Buff.PreGiveHandle);
	}
	if (Buff.PostGiveHandle.IsValid())
	{
		OnPostGiveDamage.Remove(Buff.PostGiveHandle);
	}
	ActiveBuffs.RemoveAt(Index);
}

void UComponent_Status::ClearDebuffs()
{
	for (int32 EffectIndex = ActiveStatusEffects.Num() - 1; EffectIndex >= 0; --EffectIndex)
	{
		if (ActiveStatusEffects[EffectIndex].Polarity != EStatusPolarity::Debuff)
		{
			continue;
		}
		RemoveStatusEffectAtIndex(EffectIndex);
	}

	bool bRemovedLegacyDebuff = false;
	for (int32 BuffIndex = ActiveBuffs.Num() - 1; BuffIndex >= 0; --BuffIndex)
	{
		if (ActiveBuffs[BuffIndex].bIsDebuff)
		{
			RemoveLegacyBuffAt(BuffIndex);
			bRemovedLegacyDebuff = true;
		}
	}
	if (bRemovedLegacyDebuff)
	{
		OnBuffListChanged.Broadcast(false);
	}
}

void UComponent_Status::ClearTurnBasedBuffs()
{
	RemoveTurnOnlyStatusEffects();

	const bool bHadLegacyBuffs = !ActiveBuffs.IsEmpty();
	for (int32 BuffIndex = ActiveBuffs.Num() - 1; BuffIndex >= 0; --BuffIndex)
	{
		RemoveLegacyBuffAt(BuffIndex);
	}
	if (bHadLegacyBuffs)
	{
		OnBuffListChanged.Broadcast(false);
	}
}

void UComponent_Status::CheckAttackerPreBuff(AActor* Target, int32 InDmg, int32& OutDmg)
{
	if (OnPreGiveDamage.IsBound())
	{
		OnPreGiveDamage.Broadcast(Target, InDmg, OutDmg);
	}
}

void UComponent_Status::CheckAttackerPostBuff(AActor* Target, int32 DealtDmg)
{
	if (OnPostGiveDamage.IsBound())
	{
		OnPostGiveDamage.Broadcast(Target, DealtDmg);
	}
}

void UComponent_Status::ApplyCC(FCCStructure CC)
{
	if (bIsDead)
	{
		return;
	}
	AppliedCC = CC;
	bIsOnCC = true;
	CCDuration = FMath::Clamp(AppliedCC.CCDuration, 0, MAX_CCDURATION);
	OnCCActived.Broadcast();
}

void UComponent_Status::RemoveCC()
{
	AppliedCC = FCCStructure();
	CCDuration = 0;
	bIsOnCC = false;
	OnCCRemove.Broadcast();
}

void UComponent_Status::DecreaseCCDuration(int32 WantToDecreaseCCDuration)
{
	if (!bIsOnCC || WantToDecreaseCCDuration <= 0)
	{
		return;
	}

	CCDuration = FMath::Clamp(CCDuration - WantToDecreaseCCDuration, 0, MAX_CCDURATION);
	if (CCDuration <= 0)
	{
		RemoveCC();
	}
}
